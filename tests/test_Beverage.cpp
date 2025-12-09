#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include "../qmlproject/logic/Beverage.h"

using namespace Logic;

TEST_CASE("Beverage brewingTime basic") {
    Beverage bev("Test",
                 5.0f, 0.0f, 10.0f,
                 5.0f, 0.0f, 10.0f,
                 5.0f, 0.0f, 10.0f,
                 5.0f, 0.0f, 10.0f,
                 5.0f, 0.0f, 10.0f);
    int t = bev.brewingTime();
    // 5 ingredientes: each 0.5 + normalized*10.0 -> with current=5, min=0, max=10 => normalized=0.5
    // per ingredient: 0.5 + 0.5*10 = 5.5 seconds => total 5*5.5 = 27.5s => 27500 ms
    REQUIRE(t == 27500);
}
