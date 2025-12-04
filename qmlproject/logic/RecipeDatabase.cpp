#include "RecipeDatabase.h"

namespace Logic
{

    std::vector<Beverage *> RecipeDatabase::getAllDefaultRecipes()
    {
        std::vector<Beverage *> recipes;

        // Name,
        // Coffee(val, min, max),
        // Cocoa(val, min, max),
        // Water(val, min, max),
        // Foam(val, min, max),
        // Milk(val, min, max)

        // ---------------------------------------------------------
        // ESPRESSO
        // ---------------------------------------------------------
        recipes.push_back(new Beverage(
            "Espresso",
            10.0f, 7.0f, 14.0f,
            0.0f, 0.0f, 0.0f,
            30.0f, 20.0f, 50.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f));

        // ---------------------------------------------------------
        // CAPPUCCINO
        // ---------------------------------------------------------
        recipes.push_back(new Beverage(
            "Cappuccino",
            9.0f, 7.0f, 14.0f,
            0.0f, 0.0f, 5.0f,
            30.0f, 20.0f, 50.0f,
            50.0f, 20.0f, 80.0f,
            50.0f, 0.0f, 100.0f));

        // ---------------------------------------------------------
        // AMERICANO
        // ---------------------------------------------------------
        recipes.push_back(new Beverage(
            "Americano",
            10.0f, 7.0f, 14.0f,
            0.0f, 0.0f, 0.0f,
            150.0f, 100.0f, 250.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f));

        // ---------------------------------------------------------
        // LATTE
        // ---------------------------------------------------------
        recipes.push_back(new Beverage(
            "Latte",
            8.0f, 7.0f, 14.0f,
            0.0f, 0.0f, 5.0f,
            30.0f, 20.0f, 50.0f,
            30.0f, 0.0f, 60.0f,
            150.0f, 50.0f, 250.0f));

        // ---------------------------------------------------------
        // Mocha
        // ---------------------------------------------------------
        recipes.push_back(new Beverage(
            "Mocha",
            10.0f, 7.0f, 14.0f,
            10.0f, 5.0f, 20.0f,
            30.0f, 20.0f, 50.0f,
            20.0f, 0.0f, 50.0f,
            50.0f, 20.0f, 100.0f));

        return recipes;
    }

}
