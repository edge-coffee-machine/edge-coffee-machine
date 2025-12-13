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
        // According to EdgeCoffeeMachine.cpp constructor:
        // Status should be "Idle"
        // isMakingDrink should be false
        // User should be nullptr
        CHECK(machine.status.value() == "Idle");
        CHECK(machine.getIsMakingDrink() == false);
        CHECK(machine.getUser() == nullptr);
        
        // It should have loaded the default popularity list
        CHECK(machine.getPopularBeverages().size() > 0);
    }

    SECTION("Beverage Selection: Updates public properties") {
        // Get a valid drink from the public list
        const auto& drinks = machine.getPopularBeverages();
        REQUIRE(drinks.size() > 0);
        Beverage* espresso = drinks[0]; 

        // ACTION: Select the beverage
        machine.selectBeverage(espresso);

        // VERIFY: Public properties match the selection
        CHECK(machine.getSelectedBeverage() == espresso);
        
        // Verify status text contains the drink name (e.g., "Selected: Espresso")
        // We search for the substring because the exact message might change.
        CHECK(machine.status.value().find(espresso->name.value()) != std::string::npos);
    }

    SECTION("Brewing Start: Verifies state transition to 'Busy'") {
        // 1. Select a drink
        Beverage* latte = machine.getPopularBeverages()[0];
        machine.selectBeverage(latte);

        // 2. Start Brewing
        machine.makeDrink(nullptr); 

        // VERIFY: The machine enters the brewing state
        // We can check this public property
        CHECK(machine.getIsMakingDrink() == true);
        
        // Verify status indicates activity (e.g., "Making Latte...")
        CHECK(machine.status.value().find("Making") != std::string::npos);
        CHECK(machine.status.value().find(latte->name.value()) != std::string::npos);

        // NOTE: Without access to private m_brewTimer, we stop testing here.
        // We cannot force the brew to finish, so we assume the timer started internally.
    }

    SECTION("User Session: Login and Logout public flow") {
        // Create a temporary user for testing
        // We pass empty recipes as we don't need to test the inner User logic here, 
        // just the Machine's handling of the pointer.
        std::vector<Beverage*> userRecipes; 
        User* batman = new User("Batman", 1, userRecipes);

        // 1. Login
        machine.setUser(batman);
        
        // Verify state changes
        CHECK(machine.getUser() == batman);
        CHECK(machine.status.value().find("Batman") != std::string::npos);

        // 2. Logout
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