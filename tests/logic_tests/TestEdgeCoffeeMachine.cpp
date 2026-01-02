#include "catch.hpp"
#include <string>
#include <vector>

// Include the header normally.
#include "../../qmlproject/logic/EdgeCoffeeMachine.h"

using namespace Logic;

TEST_CASE("EdgeCoffeeMachine Public Interface", "[Logic][ECM]") {
    SECTION("Initialization: Default state check") {
        EdgeCoffeeMachine& machine = EdgeCoffeeMachine::instance();

        CHECK(machine.status.value() == "Idle");
        CHECK(machine.getIsMakingDrink() == false);
        CHECK(machine.getUser() == nullptr);
        CHECK(machine.getPopularBeverages().size() > 0);
        CHECK(machine.getSelectedBeverage() != nullptr);
    }

    SECTION("Beverage Selection: Updates public properties") {
        EdgeCoffeeMachine& machine = EdgeCoffeeMachine::instance();

        const auto& drinks = machine.getPopularBeverages();
        REQUIRE(drinks.size() >= 2);

        Beverage* beverage = drinks[1];
        machine.selectBeverage(beverage);

        CHECK(machine.getSelectedBeverage() == beverage);
        // Check that the machine status contains the strings "Selected" and the name of the drink
        CHECK(machine.status.value().find("Selected") != std::string::npos);
        CHECK(machine.status.value().find(beverage->name.value()) != std::string::npos);
    }

    SECTION("Brewing Start: Verifies state transition to 'Making'") {
        EdgeCoffeeMachine& machine = EdgeCoffeeMachine::instance();

        Beverage* drink = machine.getPopularBeverages()[0];
        machine.selectBeverage(drink);
        machine.makeDrink(nullptr);

        CHECK(machine.getIsMakingDrink() == true);
        // Check that the machine status contains the strings "Making" and the name of the drink
        CHECK(machine.status.value().find("Making") != std::string::npos);
        CHECK(machine.status.value().find(drink->name.value()) != std::string::npos);
        machine.stopBrewing();
    }

    SECTION("User Session: Login and Logout public flow") {
        EdgeCoffeeMachine& machine = EdgeCoffeeMachine::instance();
        // Create a temporary user for testing
        // We pass empty recipes as we don't need to test the inner User logic here, 
        // just the Machine's handling of the pointer.
        std::vector<Beverage*> userRecipes = machine.getPopularBeverages();
        User* batman = new User("Mr.", "Batman", 1, userRecipes);

        machine.setUser(batman);

        CHECK(machine.getUser() == batman);
        // Check that the machine status contains some welcoming message including "Batman"
        CHECK(machine.status.value().find("Mr.") != std::string::npos);

        machine.logoutUser();

        CHECK(machine.getUser() == nullptr);
        CHECK(machine.getUser() == nullptr);

        delete batman;
    }

    SECTION("Error Handling: Attempting to brew without selection") {
        EdgeCoffeeMachine& machine = EdgeCoffeeMachine::instance();

        machine.selectBeverage(nullptr);
        machine.makeDrink(nullptr);

        CHECK(machine.getIsMakingDrink() == false);
        CHECK(machine.status.value() == "Invalid beverage.");
    }
}
