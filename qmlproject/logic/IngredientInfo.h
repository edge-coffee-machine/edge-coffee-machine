#ifndef INGREDIENTINFO_H
#define INGREDIENTINFO_H

#include <Qul/Object.h>
#include <Qul/Property.h>

/* Struct to hold information about an ingredient
 * Current and def variables are normalized values between 0 and 1, where:
 * - **0** corresponds to the minimum amount of the ingredient.
 * - **1** corresponds to the maximum amount of the ingredient.
 *
 * The normalization is based on absolute minimum and maximum values
 * The min and max variables are not normalized and represent the
 * actual limits for the ingredient in a beverage.
 * 
 * The def variable holds the default normalized value for the ingredient,which can not
 * be changed. The current variable holds the current normalized value for the ingredient,
 * which is modified when the user brews a beverage with modified ingredients through the interface. The modification is 
 * done through a function in the Beverage class.
 * 
 * ### Attributes:
 * - **current**: The current normalized amount of the ingredient (0 to 1).
 * - **min**: The absolute minimum amount of the ingredient (not normalized).
 * - **max**: The absolute maximum amount of the ingredient (not normalized).
 * - **def**: The default normalized amount of the ingredient (0 to 1).
*/

namespace Logic{
    class IngredientInfo : public Qul::Object
    {
    public:
            // Constructor
            IngredientInfo() : m_current(0.0f), m_min(0.0f), m_max(0.0f), m_def(0.0f) {
                current.setValue(m_current);
                min.setValue(m_min);
                max.setValue(m_max);
                def.setValue(m_def);
            }

            void setup(float minVal, float maxVal, float defVal) {
                m_min = minVal;
                m_max = maxVal;
                m_def = defVal;
                m_current = defVal;

                min.setValue(m_min);
                max.setValue(m_max);
                def.setValue(m_def);
                current.setValue(m_current);
            }

            Qul::Property<float> current;
            Qul::Property<float> min;
            Qul::Property<float> max;
            Qul::Property<float> def;

        private:
            float m_current;
            float m_min;
            float m_max;
            float m_def;
    };
}
#endif // INGREDIENTINFO_H
