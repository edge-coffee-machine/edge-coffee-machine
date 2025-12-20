#include "catch.hpp"
#include <string>
#include <vector>

// Include the header normally. No hacks, no friends.
#include "../qmlproject/logic/EdgeCoffeeMachine.h" 

using namespace Logic;

TEST_CASE("EdgeCoffeeMachine Public Interface", "[Logic][ECM]") {

    // Get reference to the singleton
    EdgeCoffeeMachine& machine = EdgeCoffeeMachine::instance();

    // Helper to reset machine state to a known clean slate using ONLY public methods
    auto resetMachine = [&]() {
        EdgeCoffeeMachine::resetInstance();
        machine = EdgeCoffeeMachine::instance();
    };

    resetMachine();

    SECTION("Initialization: Default state check") {
        CHECK(machine.status.value() == "Idle");
        CHECK(machine.getIsMakingDrink() == false);
        CHECK(machine.getUser() == nullptr);
        CHECK(machine.getSelectedBeverage() != nullptr);
        CHECK(machine.getPopularBeverages().size() > 0);
    }

    SECTION("Beverage Selection: Updates public properties") {
        // Get the drink list
        const auto& drinks = machine.getPopularBeverages(); 
        REQUIRE(drinks.size() >= 2);

        // Get the first drink
        Beverage* beverage = drinks[0]; 
        
        // Select it
        machine.selectBeverage(beverage);

        // Verify selection
        CHECK(machine.getSelectedBeverage() == beverage); 

        // Verify status text contains the drink name (e.g., "Selected: Espresso")
        // We search for the substring because the exact message might change.
        CHECK(machine.status.value().find("Selected") != std::string::npos);
        CHECK(machine.status.value().find(beverage->name.value()) != std::string::npos);
    }

    SECTION("Brewing Start: Verifies state transition to 'Making'") {
        // Select a drink
        Beverage* drink = machine.getPopularBeverages()[0];
        machine.selectBeverage(drink);

        // Start Brewing
        machine.makeDrink(nullptr); 

        // VERIFY: The machine enters the brewing state
        // We can check this public property
        CHECK(machine.getIsMakingDrink() == true);
        
        // Verify status indicates activity (e.g., "Making Latte...")
        CHECK(machine.status.value().find("Making") != std::string::npos);
        CHECK(machine.status.value().find(drink->name.value()) != std::string::npos);
    }

    SECTION("User Session: Login and Logout public flow") {
        // Create a temporary user for testing
        // We pass empty recipes as we don't need to test the inner User logic here, 
        // just the Machine's handling of the pointer.
        std::vector<Beverage*> userRecipes; 
        User* batman = new User("Batman", 1, userRecipes);

        // Login
        machine.setUser(batman);
        
        // Verify state changes
        CHECK(machine.getUser() == batman);
        CHECK(machine.status.value().find("Batman") != std::string::npos);

        // Logout
        machine.logoutUser();

        // Verify return to Guest state
        CHECK(machine.getUser() == nullptr);
        CHECK(machine.status.value() == "Ready");

        // Cleanup manually allocated user
        delete batman;
    }

    SECTION("Error Handling: Attempting to brew without selection") {
        // Reset to ensure nothing is selected
        machine.selectBeverage(nullptr);

        // Try to brew
        machine.makeDrink(nullptr);

        // Verify machine refused to start
        CHECK(machine.getIsMakingDrink() == false);
        // Status should indicate error or remain Idle (depending on implementation)
        // Based on your cpp: status.setValue("Invalid beverage.");
        CHECK(machine.status.value() == "Invalid beverage.");
    }
}