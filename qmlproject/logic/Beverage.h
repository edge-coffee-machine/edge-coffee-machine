#ifndef BEVERAGE_H
#define BEVERAGE_H

#include "IngredientInfo.h"

#include <Qul/Object.h>
#include <Qul/Property.h>

#include <string>

namespace Logic
{
    /**
     * @class Beverage
     * @brief Represents a customizable drink configuration.
     *
     * The `Beverage` class defines the structure of a drink (e.g., Espresso, Cappuccino)
     * by aggregating specific ingredients such as coffee, milk, and water.
     *
     * Each ingredient is managed by an `IngredientInfo` object, which defines the
     * minimum, maximum, and default values. These ingredients are exposed as
     * properties to allow direct interaction and binding from the user interface.
     */
    class Beverage : public Qul::Object
    {
    public:
        /**
         * @brief Constructs a new Beverage with specific ingredient limits and defaults.
         *
         * Initializes the beverage name and configures the internal storage for all
         * supported ingredients. It sets the range (min/max) and the default starting
         * value for each component.
         *
         * @param nameVal The display name of the beverage.
         * @param coffeeBeansVal Initial/Default amount of coffee beans.
         * @param minCoffeeBeans Minimum allowed amount for coffee beans.
         * @param maxCoffeeBeans Maximum allowed amount for coffee beans.
         * @param cocoaPowderVal Initial/Default amount of cocoa powder.
         * @param minCocoaPowder Minimum allowed amount for cocoa powder.
         * @param maxCocoaPowder Maximum allowed amount for cocoa powder.
         * @param waterVal Initial/Default amount of water.
         * @param minWater Minimum allowed amount for water.
         * @param maxWater Maximum allowed amount for water.
         * @param foamVal Initial/Default amount of foam.
         * @param minFoam Minimum allowed amount for foam.
         * @param maxFoam Maximum allowed amount for foam.
         * @param milkVal Initial/Default amount of milk.
         * @param minMilk Minimum allowed amount for milk.
         * @param maxMilk Maximum allowed amount for milk.
         */
        Beverage(
            const std::string &nameVal,
            float coffeeBeans, float minCoffeeBeans, float maxCoffeeBeans,
            float cocoaPowder, float minCocoaPowder, float maxCocoaPowder,
            float water, float minWater, float maxWater,
            float foam, float minFoam, float maxFoam,
            float milk, float minMilk, float maxMilk);

        /**
         * @brief The display name of the beverage.
         */
        Qul::Property<std::string> name;

        /**
         * @brief Property pointers to ingredient details.
         *
         * These properties allow the UI to access and modify the specific
         * ingredient parameters (current value, min, max) directly.
         */
        Qul::Property<IngredientInfo *> coffeeBeans;
        Qul::Property<IngredientInfo *> cocoaPowder;
        Qul::Property<IngredientInfo *> water;
        Qul::Property<IngredientInfo *> foam;
        Qul::Property<IngredientInfo *> milk;

        /**
         * @brief Resets all ingredients to their default configuration.
         *
         * Iterates through all ingredients (coffee, milk, water, etc.) and restores
         * their `current` value to the defined `def` (default) value.
         */
        void resetIngredients()
        {
            m_coffeeBeans.current.setValue(m_coffeeBeans.def.value());
            m_cocoaPowder.current.setValue(m_cocoaPowder.def.value());
            m_water.current.setValue(m_water.def.value());
            m_foam.current.setValue(m_foam.def.value());
            m_milk.current.setValue(m_milk.def.value());
        }

        /**
         * @brief Calculates the estimated brewing time.
         *
         * Computes the total time required to prepare the beverage based on the
         * current quantity of each active ingredient.
         *
         * @return The estimated time in milliseconds.
         */
        int brewingTime();

    private:
        // Internal storage for ingredient data.
        // The public Qul::Property pointers above link to these objects
        IngredientInfo m_coffeeBeans;
        IngredientInfo m_cocoaPowder;
        IngredientInfo m_water;
        IngredientInfo m_foam;
        IngredientInfo m_milk;
    };
}
#endif // BEVERAGE_H
