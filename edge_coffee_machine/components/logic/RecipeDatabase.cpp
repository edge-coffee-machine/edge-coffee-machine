/**
 * @file RecipeDatabase.cpp
 * @brief Implementation of the static recipe factory.
 */

#include "RecipeDatabase.h"

namespace Logic
{
    /**
     * @brief Generates and returns the complete list of factory-default beverages.
     *
     * This static factory method instantiates the "Master" recipes available on the
     * coffee machine. It creates `Beverage` objects on the heap, configuring each one
     * with specific presets for ingredients (Coffee, Cocoa, Water, Foam, Milk) based
     * on standard recipes.
     *
     * The following beverages are initialized:
     * - **Espresso**: High coffee concentration, low water.
     * - **Cappuccino**: Balanced mix of coffee, water, milk, and foam.
     * - **Americano**: Coffee diluted with a large amount of water.
     * - **Latte**: Coffee with a high volume of milk and some foam.
     * - **Mocha**: A complex mix including Cocoa powder.
     *
     * @return A `std::vector` containing pointers to the newly allocated `Beverage` objects.
     */
    std::vector<Beverage *> RecipeDatabase::getAllDefaultRecipes()
    {
        std::vector<Beverage *> recipes;

        // Parameter order for Beverage constructor:
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
