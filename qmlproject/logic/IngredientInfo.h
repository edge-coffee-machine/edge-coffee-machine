#ifndef INGREDIENTINFO_H
#define INGREDIENTINFO_H

#include <QObject>
#include <QVariantMap>

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

struct IngredientInfo {
    Q_GADGET
    Q_PROPERTY(float current READ current)
    Q_PROPERTY(float min READ min)
    Q_PROPERTY(float max READ max)
    Q_PROPERTY(float def READ def)

public:
    float current() const { return m_current; }
    float min() const { return m_min; }
    float max() const { return m_max; }
    float def() const { return m_def; }

    float m_current = 0.0f;
    float m_min = 0.0f;
    float m_max = 0.0f;
    float m_def = 0.0f;
};

Q_DECLARE_METATYPE(IngredientInfo)

#endif // INGREDIENTINFO_H