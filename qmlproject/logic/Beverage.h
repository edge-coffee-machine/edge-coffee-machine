#ifndef BEVERAGE_H
#define BEVERAGE_H

#include <Qul/Object.h>
#include <Qul/Property.h>
#include <string>

#include "IngredientInfo.h"

/**
 * @class Beverage
 * @brief Represents a beverage with configurable ingredients.
 *
 * The `Beverage` class models a drink with various ingredients
 * that are stored in a map. Each ingredient is represented by a name and
 * a IngredientInfo struct that holds information about the ingredient's.
 * The class also provides the static list of the default beverages.
 *
 * ### Attributes:
 * - **name**: The name of the beverage (read-only).
 * - **ingredients**: A map of ingredient names to their corresponding IngredientInfo.
 * - **defaultBeverageList**: A static list of default beverage instances.
 */

namespace Logic
{
    class Beverage : public Qul::Object
    {
    public:
        Beverage(
            const std::string &nameVal,
            float coffeeBeans, float minCoffeeBeans, float maxCoffeeBeans,
            float cocoaPowder, float minCocoaPowder, float maxCocoaPowder,
            float water, float minWater, float maxWater,
            float foam, float minFoam, float maxFoam,
            float milk, float minMilk, float maxMilk); // Constructor

        Qul::Property<std::string> name;

        Qul::Property<IngredientInfo *> coffeeBeans;
        Qul::Property<IngredientInfo *> cocoaPowder;
        Qul::Property<IngredientInfo *> water;
        Qul::Property<IngredientInfo *> foam;
        Qul::Property<IngredientInfo *> milk;

        IngredientInfo *coffeeBeansPtr() { return &m_coffeeBeans; }
        IngredientInfo *cocoaPowderPtr() { return &m_cocoaPowder; }
        IngredientInfo *waterPtr() { return &m_water; }
        IngredientInfo *foamPtr() { return &m_foam; }
        IngredientInfo *milkPtr() { return &m_milk; }

        void resetIngredients()
        {
            m_coffeeBeans.current.setValue(m_coffeeBeans.def.value());
            m_cocoaPowder.current.setValue(m_cocoaPowder.def.value());
            m_water.current.setValue(m_water.def.value());
            m_foam.current.setValue(m_foam.def.value());
            m_milk.current.setValue(m_milk.def.value());
        }

        int brewingTime();

    private:
        IngredientInfo m_coffeeBeans;
        IngredientInfo m_cocoaPowder;
        IngredientInfo m_water;
        IngredientInfo m_foam;
        IngredientInfo m_milk;
    };
}
#endif // BEVERAGE_H
