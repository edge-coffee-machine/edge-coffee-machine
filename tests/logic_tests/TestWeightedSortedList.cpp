#include "catch.hpp"
#include <string>
#include <vector>
#include "../../qmlproject/logic/WeightedSortedList.h"

using namespace Logic;

TEST_CASE("WeightedSortedList Container Logic", "[Logic][WeightedSortedList]") {

    // Helper to verify weights sum to 1.0 (approximately)
    auto checkSum = [](const WeightedSortedList<std::string>& list) {
        float sum = 0.0f;
        for (float w : list.weights()) {
            sum += w;
        }
        REQUIRE(sum == Approx(1.0f));
    };

    SECTION("Initialization: Items start with uniform weights") {
        std::vector<std::string> items = {"Espresso", "Latte", "Water", "Tea"};
        // Create list with default learning rate
        WeightedSortedList<std::string> list(items, 0.2f); 

        REQUIRE(list.size() == 4);
        
        // Expected initial weight: 1.0 / 4 = 0.25
        for (int i = 0; i < list.size(); ++i) {
            CHECK(list.weightAt(i) == Approx(0.25f));
        }
        
        checkSum(list);
    }

    SECTION("Selection Logic: Weight updates and sorting") {
        // Scenario: 
        // We use a high learning rate (0.5) to make manual calculation easy.
        // Initial state: ["A", "B"] -> Weights: [0.5, 0.5]
        
        std::vector<std::string> data = {"A", "B"};
        float learningRate = 0.5f;
        WeightedSortedList<std::string> list(data, learningRate);

        // Action: Select "B" (currently at index 1)
        // Formula: 
        //   Selected (B): R + (1-R)*Old = 0.5 + (0.5 * 0.5) = 0.75
        //   Others   (A): (1-R)*Old     = 0.5 * 0.5 = 0.25
        list.recordSelection("B");

        // CHECK 1: Reordering
        // "B" (0.75) is now heavier than "A" (0.25), so it should be at index 0.
        REQUIRE(list.items()[0] == "B");
        REQUIRE(list.items()[1] == "A");
        CHECK(list.indexOf("B") == 0);
        CHECK(list.indexOf("A") == 1);

        // CHECK 2: Weight Values
        CHECK(list.weightAt(0) == Approx(0.75f)); // B's weight
        CHECK(list.weightFor("B") == Approx(0.75f)); // Should work with weightFor as well
        CHECK(list.weightAt(1) == Approx(0.25f)); // A's weight
        CHECK(list.weightFor("A") == Approx(0.25f)); // A's weight

        
        checkSum(list);
    }

    SECTION("Progression: Item bubbles up correctly") {
        // Scenario: 3 items. Select the last one multiple times to see it rise.
        // Initial: [A, B, C] -> Weights approx 0.33 each
        std::vector<std::string> data = {"A", "B", "C"};
        WeightedSortedList<std::string> list(data, 0.3f);

        // Select "C" (Index 2)
        list.recordSelection("C");
        
        // "C" gains weight. Depending on the math, it might not pass "A" or "B" immediately 
        // if the rate is low, or it might jump to top if rate is high.
        // With R=0.3:
        // C new = 0.3 + 0.7*0.333 = 0.533
        // A, B new = 0.7*0.333 = 0.233
        // So C should jump to top immediately.
        
        REQUIRE(list.items()[0] == "C");
        REQUIRE(list.weightAt(0) > list.weightAt(1));
        REQUIRE(list.weightAt(1) == list.weightAt(2)); // A and B should degrade equally
    }

    SECTION("Edge Cases: Empty list handling") {
        std::vector<std::string> empty;
        WeightedSortedList<std::string> list(empty);

        REQUIRE(list.size() == 0);
        REQUIRE(list.indexOf("Ghost") == -1);
        
        // Should not crash
        list.recordSelectionAt(0);
        list.recordSelection("Ghost");
    }

    SECTION("Configuration: Changing Learning Rate") {
        std::vector<std::string> data = {"X", "Y"};
        WeightedSortedList<std::string> list(data, 0.1f);
        
        REQUIRE(list.weightR() == 0.1f);
        
        list.setWeightR(0.9f);
        REQUIRE(list.weightR() == 0.9f);
        
        // With rate 0.9, selection should dominate almost completely
        // Initial: 0.5, 0.5
        // Select X: New = 0.9 + 0.1*0.5 = 0.95
        list.recordSelection("X");
        
        CHECK(list.weightAt(0) == Approx(0.95f));
    }
}