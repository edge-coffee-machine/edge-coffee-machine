#include "Beverage.h"

namespace Logic
{
  /**
   * @brief Constructs a new Beverage object with specific ingredient configurations.
   * * This constructor initializes the beverage's name and configures the limits (min/max)
   * and initial values for all supported ingredients. It also binds the internal
   * storage objects (m_coffeeBeans, etc.) to the public Qul::Property pointers
   * to ensure they are accessible and observable from the QML interface.
   * * @param nameVal The display name of the beverage (e.g., "Cappuccino").
   * @param coffeeBeansVal Initial amount of coffee beans.
   * @param minCoffeeBeans Minimum allowed amount for coffee beans.
   * @param maxCoffeeBeans Maximum allowed amount for coffee beans.
   * @param cocoaPowderVal Initial amount of cocoa powder.
   * @param minCocoaPowder Minimum allowed amount for cocoa powder.
   * @param maxCocoaPowder Maximum allowed amount for cocoa powder.
   * @param waterVal Initial amount of water.
   * @param minWater Minimum allowed amount for water.
   * @param maxWater Maximum allowed amount for water.
   * @param foamVal Initial amount of foam.
   * @param minFoam Minimum allowed amount for foam.
   * @param maxFoam Maximum allowed amount for foam.
   * @param milkVal Initial amount of milk.
   * @param minMilk Minimum allowed amount for milk.
   * @param maxMilk Maximum allowed amount for milk.
   */
  Beverage::Beverage(const std::string &nameVal,
                     float coffeeBeansVal, float minCoffeeBeans, float maxCoffeeBeans,
                     float cocoaPowderVal, float minCocoaPowder, float maxCocoaPowder,
                     float waterVal, float minWater, float maxWater,
                     float foamVal, float minFoam, float maxFoam,
                     float milkVal, float minMilk, float maxMilk)
  {
    name.setValue(nameVal);

    m_coffeeBeans.setup(minCoffeeBeans, maxCoffeeBeans, coffeeBeansVal);
    m_cocoaPowder.setup(minCocoaPowder, maxCocoaPowder, cocoaPowderVal);
    m_water.setup(minWater, maxWater, waterVal);
    m_foam.setup(minFoam, maxFoam, foamVal);
    m_milk.setup(minMilk, maxMilk, milkVal);

    coffeeBeans.setValue(&m_coffeeBeans);
    cocoaPowder.setValue(&m_cocoaPowder);
    water.setValue(&m_water);
    foam.setValue(&m_foam);
    milk.setValue(&m_milk);
  }

  /**
   * @brief Calculates the estimated brewing time based on the current ingredient values.
   * * The method iterates through all ingredients. If an ingredient is active (range > 0),
   * it contributes to the total time based on a normalized formula:
   * - Base time per ingredient: 0.5 seconds.
   * - Variable time: Up to 10.0 seconds, proportional to the current value's
   * position between min and max (normalized 0.0 to 1.0).
   * * Formula: TotalTime += 0.5 + (NormalizedValue * 10.0)
   * * @return The calculated brewing time in milliseconds.
   */
  int Beverage::brewingTime()
  {
    float totalTime = 0.0f;

    auto addContribution = [&](IngredientInfo &info)
    {
      float min = info.min.value();
      float max = info.max.value();
      float current = info.current.value();
      float range = max - min;

      if (range > 0.001f)
      {
        float normalized = (current - min) / range;
        totalTime += 0.5f + (normalized * 10.0f);
      }
    };

    addContribution(m_coffeeBeans);
    addContribution(m_cocoaPowder);
    addContribution(m_water);
    addContribution(m_foam);
    addContribution(m_milk);

    return static_cast<int>(totalTime * 1000);
  };
}
