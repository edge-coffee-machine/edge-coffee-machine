#include "catch.hpp"

#include <string>
#include <vector>
#include <iostream>

// Include the class to test
#include "../../qmlproject/logic/User.h"
#include "../../qmlproject/logic/Beverage.h"
#include "../../qmlproject/logic/EdgeCoffeeMachine.h"

using namespace Logic;

// Helper to create a fresh set of dummy beverages
std::vector<Beverage*> createTestRecipes() {
    std::vector<Beverage*> recipes;
    // We create 5 drinks. 
    // Note: User class takes ownership and will delete these in its destructor.
    recipes.push_back(new Beverage("Espresso",  5,0,10, 0,0,0, 5,0,10, 0,0,0, 0,0,0));
    recipes.push_back(new Beverage("Americano", 5,0,10, 0,0,0, 8,0,10, 0,0,0, 0,0,0));
    recipes.push_back(new Beverage("Latte",     5,0,10, 0,0,0, 5,0,10, 5,0,10, 5,0,10));
    recipes.push_back(new Beverage("Cappuccino",5,0,10, 0,0,0, 5,0,10, 8,0,10, 5,0,10));
    recipes.push_back(new Beverage("Macchiato", 5,0,10, 5,0,10, 5,0,10, 5,0,10, 5,0,10));
    recipes.push_back(new Beverage("Mocha", 5,0,10, 5,0,10, 5,0,10, 5,0,10, 5,0,10));
    return recipes;
}

TEST_CASE("User Profile and Recommendation Engine", "[Logic][User]") {
    EdgeCoffeeMachine::resetInstance();
    EdgeCoffeeMachine::instance();

    // SETUP: Create recipes and user
    // The vector 'recipes' passes ownership of pointers to 'user'
    std::vector<Beverage*> recipes = createTestRecipes();
    
    // Keep raw pointers to specific drinks for validation before giving them to User
    Beverage* espresso = recipes[0];   // Index 0
    Beverage* americano = recipes[1];  // Index 1
    Beverage* macchiato = recipes[4];  // Index 4

    Logic::User user("Mr.", "Batman", 1, recipes);

    SECTION("Initialization: User starts as Default") {
        REQUIRE(user.name.value() == "Mr.");
        REQUIRE(user.surname.value() == "Batman");
        REQUIRE(user.initials.value() == "MB");
        REQUIRE(user.category() == User::UserCategory::Default);
        
        // In Default mode, weights haven't diverged yet
        const auto& displayList = user.getDisplayBeverages();
        REQUIRE(displayList.size() == 6);
    }

    SECTION("Scenario: Conservative User (Habitual behavior)") {
        // Conservative behavior: 
        // 1. Sticks to the same drink (Espresso)
        // 2. Does NOT customize
        // 3. Needs > 3 drinks to be classified
        
        INFO("Brewing Espresso 3 times without customization");
        
        for(int i=0; i<3; i++) {
            user.beverageBrewed(espresso);
        }
        
        // CHECK 1: Classification
        // Low variety (TryerScore low) + No customization (CustomizerScore low) -> Conservative
        REQUIRE(user.category() == User::UserCategory::Conservative);
        
        // CHECK 2: Sorting
        // Conservative users see list sorted strictly by weight (Favorites first).
        // Espresso was selected 4 times, so it must be #1.
        const auto& list = user.getDisplayBeverages();
        REQUIRE(list.front()->name.value() == "Espresso");
        
        // Ensure weights are decaying correctly (others should be lower than Espresso)
        // Espresso weight > Americano weight
        // Accessing internal weights indirectly via sort order
        CHECK(list[0] == espresso); 
        CHECK(list[1] != espresso);
    }

    SECTION("Scenario: Early Adopter (Variety & Customization)") {
        // Early Adopter behavior:
        // 1. Tries different drinks (High Tryer Score)
        // 2. Customizes drinks (High Customizer Score)
        
        INFO("Brewing different drinks with customization");
        
        // Brew 1: Custom Espresso
        user.beverageCustomized(); 
        user.beverageBrewed(espresso); // Index 0
        
        // Brew 2: Custom Americano
        user.beverageCustomized();
        user.beverageBrewed(americano); // Index 1
        
        // Brew 3: Custom Macchiato
        user.beverageCustomized();
        user.beverageBrewed(macchiato); // Index 4 (Novelty!)
        
        // CHECK 1: Classification
        REQUIRE(user.category() == User::UserCategory::EarlyAdopter);
        
        // CHECK 2: The "Suggestion" Injection logic
        // Early Adopters get a low-weight suggestion inserted at index 2 (3rd position).
        // Based on our brewing, Macchiato is high weight.
        // The list should be roughly: [High, High, SUGGESTION, ...]
        
        const auto& list = user.getDisplayBeverages();
        REQUIRE(list.size() == 6);
        
        CHECK(list[0] == macchiato);
        CHECK(list[1] == americano);
        CHECK(list[2] != espresso); // Espresso is the third most liked, but here goes the suggestion
        CHECK(list[3] == espresso);
    }

    SECTION("Error Handling: Brewing null or unknown beverage") {
        // Should not crash
        user.beverageBrewed(nullptr);
        // Create a beverage NOT in the user's list
        Beverage unknown("Unknown", 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0);
        user.beverageBrewed(&unknown);
        
        // State should remain consistent
        CHECK(user.category() == User::UserCategory::Default);
    }
}