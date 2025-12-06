#ifndef INGREDIENTINFO_H
#define INGREDIENTINFO_H

#include <qul/object.h>
#include <qul/property.h>

namespace Logic
{
    /**
     * @class IngredientInfo
     * @brief Represents the configuration and state of a single beverage ingredient.
     *
     * This class manages the parameters for a specific ingredient (e.g., Coffee Beans, Water, Milk).
     * It stores the values in **absolute units** (e.g., grams or milliliters) defining the
     * acceptable range (min/max) and the current user selection.
     */
    class IngredientInfo : public Qul::Object
    {
    public:
        /**
         * @brief Default constructor.
         *
         * Initializes all values (current, min, max, def) to 0.0f.
         */
        IngredientInfo() : m_current(0.0f), m_min(0.0f), m_max(0.0f), m_def(0.0f)
        {
            current.setValue(m_current);
            min.setValue(m_min);
            max.setValue(m_max);
            def.setValue(m_def);
        }

        /**
         * @brief Configures the ingredient with specific limits and a default value.
         *
         * Sets the absolute range and the initial value for the ingredient.
         *
         * @param minVal The minimum allowed amount (e.g., 5.0 grams).
         * @param maxVal The maximum allowed amount (e.g., 20.0 grams).
         * @param defVal The default recommended amount (e.g., 10.0 grams).
         */
        void setup(float minVal, float maxVal, float defVal)
        {
            m_min = minVal;
            m_max = maxVal;
            m_def = defVal;
            m_current = defVal;

            min.setValue(m_min);
            max.setValue(m_max);
            def.setValue(m_def);
            current.setValue(m_current);
        }

        /**
         * @brief The current selected amount of the ingredient.
         *
         * Represents the actual quantity (e.g., grams or ml) selected by the user
         * or set by the recipe. This value is bound directly to UI controls.
         */
        Qul::Property<float> current;

        /**
         * @brief The absolute minimum limit for this ingredient.
         * Defines the lower bound of the valid range.
         */
        Qul::Property<float> min;

        /**
         * @brief The absolute maximum limit for this ingredient.
         * Defines the upper bound of the valid range.
         */
        Qul::Property<float> max;

        /**
         * @brief The factory default amount for this ingredient.
         * Used to restore the standard recipe settings.
         */
        Qul::Property<float> def;

    private:
        float m_current;
        float m_min;
        float m_max;
        float m_def;
    };
}
#endif // INGREDIENTINFO_H
