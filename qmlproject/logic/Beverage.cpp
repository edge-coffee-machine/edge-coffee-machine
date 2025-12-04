#include "Beverage.h"

// Implementations for Beverage

// Constructor
namespace Logic
{

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
