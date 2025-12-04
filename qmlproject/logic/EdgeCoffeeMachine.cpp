#include "EdgeCoffeeMachine.h"

#include "BeverageListModel.h"
#include "UserModel.h"
#include "User.h"
#include <Qul/Log.h>

#include <algorithm>
#include <cstdio>

namespace Logic
{
  EdgeCoffeeMachine::EdgeCoffeeMachine()
  {
    m_brewTimer.setSingleShot(true);
    m_brewTimer.onTimeout([this]()
                          { this->finishBrewing(); });

    status.setValue("Idle");
    isMakingDrink.setValue(false);
    selectedBeverage.setValue(nullptr);
    user.setValue(nullptr);

    std::vector<Beverage *> recipes = RecipeDatabase::getAllDefaultRecipes();

    m_weightedBeverages = WeightedSortedList<Beverage *>(recipes, popularityWeightR);

    Qul::PlatformInterface::log("[ECM] Initial popular beverages and weights:\n");

    const std::vector<Beverage *> &beverages = m_weightedBeverages.items();
    const std::vector<float> &weights = m_weightedBeverages.weights();

    for (size_t i = 0; i < beverages.size(); ++i)
    {
      Qul::PlatformInterface::log("   Beverage [%d]: %s, weight = %f\n",
                                  static_cast<int>(i),
                                  beverages[i]->name.value().c_str(),
                                  static_cast<double>(weights[i]));
    }

    if (!m_weightedBeverages.items().empty())
    {
      selectBeverage(m_weightedBeverages.items().front());
    }
    else
    {
      Qul::PlatformInterface::log("[ECM] Warning: initialized with an empty beverage list!\n");
      selectedBeverage.setValue(nullptr);
    }

    updateModels();
  }

  void EdgeCoffeeMachine::recordBeverageSelection(Beverage *drink)
  {
    if (!drink)
    {
      Qul::PlatformInterface::log("[ECM] Warning: Selected beverage is null.\n");
      return;
    }

    m_weightedBeverages.recordSelection(drink);

    Qul::PlatformInterface::log("[ECM] Recorded beverage selection: %s\n",
                                drink->name.value().c_str());

    const std::vector<Beverage *> &items = m_weightedBeverages.items();
    const std::vector<float> &weights = m_weightedBeverages.weights();

    for (size_t j = 0; j < items.size(); ++j)
    {
      Qul::PlatformInterface::log("   Beverage [%d]: %s, weight = %f\n",
                                  static_cast<int>(j),
                                  items[j]->name.value().c_str(),
                                  static_cast<double>(weights[j]));
    }

    updateModels();
  }

  const std::vector<Beverage *> &EdgeCoffeeMachine::getPopularBeverages() const
  {
    return m_weightedBeverages.items();
  }

  void EdgeCoffeeMachine::setUser(User *newUser)
  {
    if (user.value() != newUser)
    {
      user.setValue(newUser);

      if (newUser)
      {
        std::string welcome = "Welcome " + newUser->name.value();
        status.setValue(welcome);

        Qul::PlatformInterface::log("[ECM] User login: %s\n", newUser->name.value().c_str());
      }
      else
      {
        status.setValue("Ready");
        Qul::PlatformInterface::log("[ECM] User logout.\n");
      }
    }

    updateModels();
    
  }

  void EdgeCoffeeMachine::makeDrink(Beverage *beverage)
  {
    if (isMakingDrink.value())
    {
      status.setValue("Already making a drink. Please wait.");
      return;
    }

    Beverage *target = beverage ? beverage : selectedBeverage.value();

    if (!target)
    {
      status.setValue("Invalid beverage.");
      return;
    }

    std::string drinkName = target->name.value();

    selectBeverage(target);

    isMakingDrink.setValue(true);
    status.setValue("Making " + drinkName + "...");

    Qul::PlatformInterface::log("[ECM] Starting to make: %s\n", drinkName.c_str());

    int timeToBrew = target->brewingTime();
    Qul::PlatformInterface::log("[ECM] Estimated brewing time (ms): %d\n", timeToBrew);

    m_brewTimer.setInterval(timeToBrew);
    m_brewTimer.start();
  }

  void EdgeCoffeeMachine::finishBrewing()
  {
    Beverage *target = selectedBeverage.value();
    if (!target)
      return;

    std::string drinkName = target->name.value();
    User *currentUser = user.value();

    beginResetModel();

    if (currentUser)
    {
      currentUser->beverageBrewed(target);
      user.setValue(nullptr);

      Qul::PlatformInterface::log("[ECM] User brewed %s and logged out.\n", drinkName.c_str());
    }
    else
    {
      m_weightedBeverages.recordSelection(target);

      Qul::PlatformInterface::log("[ECM] Guest selection recorded for %s.\n", drinkName.c_str());
    }

    endResetModel();

    status.setValue(drinkName + " is ready!");
    isMakingDrink.setValue(false);

    Qul::PlatformInterface::log("[ECM] Finished making: %s\n", drinkName.c_str());
  }

  void EdgeCoffeeMachine::selectBeverage(Beverage *beverage)
  {
    if (selectedBeverage.value() != beverage)
    {
      selectedBeverage.setValue(beverage);

      if (beverage)
      {
        status.setValue("Selected: " + beverage->name.value());

        Qul::PlatformInterface::log("[ECM] Selected: %s\n", beverage->name.value().c_str());
      }
      else
      {
        status.setValue("Select a drink");
      }
    }
  }
}
