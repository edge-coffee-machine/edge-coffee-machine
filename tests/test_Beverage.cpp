#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <string>
#include "../qmlproject/logic/Beverage.h"

using namespace Logic;

TEST_CASE("Beverage class functionality", "[Logic][Beverage]") {
    // COMMON SETUP
    // Creating a standard instance for testing. 
    // All ingredients are set to: Current/Def=5.0, Min=0.0, Max=10.0
    // Normalized value for all starts at 0.5 ((5-0)/10).
    Logic::Beverage bev("TestLatte",
                 5.0f, 0.0f, 10.0f, // Coffee Beans
                 5.0f, 0.0f, 10.0f, // Cocoa Powder
                 5.0f, 0.0f, 10.0f, // Water
                 5.0f, 0.0f, 10.0f, // Foam
                 5.0f, 0.0f, 10.0f  // Milk
    );

    SECTION("Initialization: Verifies integrity of all ingredients") {
        REQUIRE(bev.name.value() == "TestLatte");

        // Helper lambda to avoid code repetition for checking properties
        auto checkIngredient = [](IngredientInfo* ing, float expectedCur, float expectedDef, float expectedMin, float expectedMax) {
            CHECK(ing->current.value() == Approx(expectedCur));
            CHECK(ing->def.value()     == Approx(expectedDef));
            CHECK(ing->min.value()     == Approx(expectedMin));
            CHECK(ing->max.value()     == Approx(expectedMax));
        };

        // Validate Coffee Beans
        INFO("Checking Coffee Beans initialization");
        checkIngredient(bev.coffeeBeans.value(), 5.0f, 5.0f, 0.0f, 10.0f);

        // Validate Cocoa Powder
        INFO("Checking Cocoa Powder initialization");
        checkIngredient(bev.cocoaPowder.value(), 5.0f, 5.0f, 0.0f, 10.0f);

        // Validate Water
        INFO("Checking Water initialization");
        checkIngredient(bev.water.value(), 5.0f, 5.0f, 0.0f, 10.0f);

        // Validate Foam
        INFO("Checking Foam initialization");
        checkIngredient(bev.foam.value(), 5.0f, 5.0f, 0.0f, 10.0f);

        // Validate Milk
        INFO("Checking Milk initialization");
        checkIngredient(bev.milk.value(), 5.0f, 5.0f, 0.0f, 10.0f);
    }

    SECTION("Brewing Time: Calculation reflects ingredient changes") {
        // Modify Coffee Beans to 7.5
        // Range: 0-10. New Value: 7.5. Normalized: 0.75.
        bev.coffeeBeans.value()->setCurrent(7.5f);
        REQUIRE(bev.coffeeBeans.value()->current.value() == 7.5f);

        // Calculation Breakdown:
        // Formula per ingredient: Base(0.5s) + (Normalized * 10.0s)
        
        // 4 ingredients unchanged (Value 5, Norm 0.5):
        // 4 * (0.5 + (0.5 * 10)) = 4 * 5.5s = 22.0s
        float timeUnchanged = 4.0f * (0.5f + (0.5f * 10.0f));
        
        // 1 ingredient modified (Value 7.5, Norm 0.75):
        // 1 * (0.5 + (0.75 * 10)) = 1 * 8.0s = 8.0s
        float timeChanged = 1.0f * (0.5f + (0.75f * 10.0f));

        float expectedTotalSeconds = timeUnchanged + timeChanged; // 30.0s
        
        // Convert to milliseconds for the final check
        CHECK(bev.brewingTime() == static_cast<int>(expectedTotalSeconds * 1000));
    }

    SECTION("Reset Ingredients: Restores values to defaults") {
        // Modify state to ensure reset works
        bev.coffeeBeans.value()->setCurrent(10.0f);
        bev.water.value()->setCurrent(1.0f);
        bev.milk.value()->setCurrent(9.0f);
        
        // Verify state is "dirty" before resetting
        REQUIRE(bev.coffeeBeans.value()->current.value() != 5.0f);

        // Action
        bev.resetIngredients();

        // Verification: All "current" values should match "def" values (5.0f)
        CHECK(bev.coffeeBeans.value()->current.value() == 5.0f);
        CHECK(bev.water.value()->current.value() == 5.0f);
        CHECK(bev.milk.value()->current.value() == 5.0f);
        
        // Ensure untouched ingredients remain correct
        CHECK(bev.foam.value()->current.value() == 5.0f);
    }
}