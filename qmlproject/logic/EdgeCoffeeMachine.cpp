/**
 * @file EdgeCoffeeMachine.cpp
 * @brief Implementation of the central controller for the Edge Coffee Machine application.
 *
 * This file implements the core business logic, including state management,
 * beverage selection, user session handling, and the simulation of the brewing process.
 * It acts as the bridge between the data layer (User, Beverage, RecipeDatabase) and
 * the presentation layer (BeverageModel, UserModel).
 */

#include "EdgeCoffeeMachine.h"
#include "BeverageModel.h"
#include "UserModel.h"
#include "User.h"

#include <platforminterface/log.h>

#include <algorithm>
#include <cstdio>
#include <ctime>

namespace Logic
{
  /**
   * @brief Constructs the EdgeCoffeeMachine singleton and initializes the application state.
   *
   * Performs the following initialization steps:
   * 1. Configures the brew timer for simulating drink preparation.
   * 2. Sets initial state properties (status: Idle, isMakingDrink: false).
   * 3. Loads the default recipe list from the `RecipeDatabase`.
   * 4. Initializes the `WeightedSortedList` to manage beverage popularity.
   * 5. Selects the most popular beverage by default.
   * 6. Initializes the drinksList property to point to the BeverageModel instance.
   * 7. Pushes the initial data to the UI models via `updateBeverageModel()`.
   */
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

    // --- USER TEST ---
    static Qul::Timer initTimer;
    initTimer.setSingleShot(true);
    initTimer.setInterval(0); // 0ms = next event loop cycle
    initTimer.onTimeout([this]()
                        {
                          Qul::PlatformInterface::log("[ECM] Initializing test users...\n");

                          this->enrollUser(10);
                          this->enrollUser(20);

                          User *testUser = new User("Paolo", "Rossi", 1, RecipeDatabase::getAllDefaultRecipes());
                          m_user_list.push_back(testUser);
                          m_users_by_id[99] = testUser;

                          this->updateUserModel();
                        });
    initTimer.start();
    // -------------------------

    drinksList.setValue(&m_beverageModelInstance); // Initialize drinksList property to point to the BeverageModel instance
    usersList.setValue(&m_userModelInstance);      // Initialize usersList property to point to the UserModel instance

    updateModels();
  }

  /**
   * @brief Records a beverage selection to update popularity weights.
   *
   * This method is called when a drink is brewed or
   * internally to track usage statistics. It updates the weights in the
   * `WeightedSortedList`, logs the new weights for debugging, and triggers
   * a UI update to reflect any changes in the beverage sorting order.
   *
   * @param drink Pointer to the selected Beverage object. If null, the operation is ignored.
   */
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

    updateBeverageModel();
  }

  /**
   * @brief Retrieves the global list of beverages sorted by popularity.
   *
   * This helper method provides access to the raw underlying vector of beverages,
   * sorted according to the global (guest) popularity weights. It is primarily
   * used by the `User` class to initialize the default beverage list for new users.
   *
   * @return A const reference to the vector of Beverage pointers.
   */
  const std::vector<Beverage *> &EdgeCoffeeMachine::getPopularBeverages() const
  {
    return m_weightedBeverages.items();
  }

  /**
   * @brief Logs a user in or out of the machine.
   *
   * Updates the `user` property and manages the transition between the "Guest" state
   * and a specific "User" session.
   * - If `newUser` is valid, it logs the user in and updates the status message.
   * - If `newUser` is nullptr, it logs the current user out and resets to "Ready".
   *
   * Crucially, this method calls `updateBeverageModel()` to switch the displayed beverage list
   * from the global popularity list to the user's personalized list.
   *
   * @param newUser Pointer to the User object to log in, or nullptr to log out.
   */
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

    updateBeverageModel();
  }

  /**
   * @brief Initiates the brewing process for a selected beverage.
   *
   * Validates the machine state (must be idle) and the beverage selection.
   * If valid, it:
   * 1. Updates the machine state to "Making <Drink>".
   * 2. Sets `isMakingDrink` to true.
   * 3. Calculates the required brewing time based on ingredients.
   * 4. Starts the simulation timer.
   *
   * @param beverage Optional pointer to the beverage to brew. If nullptr, uses `selectedBeverage`.
   */
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

  /**
   * @brief Stops the brewing process immediately.
   *
   * Cancels the brew timer and resets the machine state to Idle.
   * If a drink was being brewed, it is considered cancelled.
   */
  void EdgeCoffeeMachine::stopBrewing()
  {
    if (isMakingDrink.value())
    {
      User *currentUser = user.value();
      std::string drinkName = selectedBeverage.value() ? selectedBeverage.value()->name.value() : "Drink";

      m_brewTimer.stop();
      isMakingDrink.setValue(false);
      status.setValue("Canceled: " + drinkName + " preparation stopped.");

      if (currentUser)
      {
        user.setValue(nullptr);
        Qul::PlatformInterface::log("[ECM] User %s canceled brewing %s and logged out.\n",
                                    currentUser->name.value().c_str(),
                                    drinkName.c_str());
      }
      else
      {
        Qul::PlatformInterface::log("[ECM] Guest canceled brewing %s.\n", drinkName.c_str());
      }
    }
    else
    {
      Qul::PlatformInterface::log("[ECM] No brewing process to cancel.\n");
    }
  }

  /**
   * @brief Completes the brewing process when the timer expires.
   *
   * This slot is called automatically by `m_brewTimer`. It handles the post-brewing logic:
   * 1. Updates user preferences (if a user is logged in) or global popularity (if guest).
   * 2. Automatically logs out the user after brewing (session end).
   * 3. Triggers `updateBeverageModel()` to refresh the UI with new weights/sorting.
   * 4. Resets the machine state to "Ready" and `isMakingDrink` to false.
   */
  void EdgeCoffeeMachine::finishBrewing()
  {
    Beverage *target = selectedBeverage.value();
    if (!target)
      return;

    std::string drinkName = target->name.value();
    User *currentUser = user.value();

    if (currentUser)
    {
      currentUser->beverageBrewed(target);
      user.setValue(nullptr);
      Qul::PlatformInterface::log("[ECM] User %s brewed %s and logged out.\n", currentUser->name.value().c_str(), drinkName.c_str());
    }
    else
    {
      Qul::PlatformInterface::log("[ECM] Guest selection recorded for %s.\n", drinkName.c_str());
    }

    recordBeverageSelection(target);

    status.setValue(drinkName + " is ready!");
    isMakingDrink.setValue(false);

    Qul::PlatformInterface::log("[ECM] Finished making: %s\n", drinkName.c_str());
  }

  /**
   * @brief Selects a beverage to be displayed or brewed.
   *
   * Updates the `selectedBeverage` property if the selection has changed.
   * This method also updates the status text to provide immediate feedback
   * to the user (e.g., "Selected: Cappuccino").
   *
   * @param beverage Pointer to the Beverage object to select.
   */
  void EdgeCoffeeMachine::selectBeverage(Beverage *beverage)
  {
    if (selectedBeverage.value() == beverage)
    {
      return;
    }

    if (!user.value() && beverage)
    {
      beverage->resetIngredients();
    }

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

  /**
   * @brief Synchronizes the backend data with the UI View Models.
   *
   * Pushes the correct list of beverages (User's list or Global list)
   * to the `BeverageModel` singleton so the UI updates.
   */
  void EdgeCoffeeMachine::updateBeverageModel()
  {
    if (user.value())
    {
      m_beverageModelInstance.updateList(user.value()->getDisplayBeverages());
      // Update the selected beverage reference to the new instance in the user list

      if (user.value()->getDisplayBeverages().size() && user.value()->getDisplayBeverages().size() > 0)
      {
        selectBeverage(user.value()->getDisplayBeverages().front());
      }
      else
      {
        selectBeverage(nullptr);
      }
    }
    else
    {
      m_beverageModelInstance.updateList(m_weightedBeverages.items());
      // Update the selected beverage reference to the new instance in the global list
      selectBeverage(m_weightedBeverages.items().front());
    }
  }

  /**
   * @brief Updates the UserModel with the complete list of registered users.
   *
   * Retrieves the full internal list of enrolled users (m_user_list) and pushes
   * it to the `UserModel` singleton. This ensures that UI components designed for
   * user selection or administration panels have access to the latest, complete set of profiles.
   */
  void EdgeCoffeeMachine::updateUserModel()
  {
    m_userModelInstance.updateList(m_user_list);
  }

  /**
   * @brief Central synchronization point for all QML View Models.
   *
   * This function orchestrates the update process whenever the application state changes.
   * It delegates the update by calling both the specialized beverage model helper and
   * the user model helper to ensure complete UI coherence across all list views.
   */
  void EdgeCoffeeMachine::updateModels()
  {
    updateBeverageModel();
    updateUserModel();
  }

  /**
   * @brief Enrolls a new user into the system with default settings.
   *
   * Creates a new `User` object with a default name and picture,
   * initializes their beverage list from the default recipe list,
   * Finally, it updates the `UserModel` to reflect the new user in the UI
   * and logs the enrollment action.
   *
   * @param id The unique identifier assigned by the AI subsystem.
   */
  void EdgeCoffeeMachine::enrollUser(int id)
  {
    if (m_users_by_id.find(id) != m_users_by_id.end())
    {
      Qul::PlatformInterface::log("[ECM] enrol id %d already exists\n", id);
      return;
    }

    // Generate username by concatenating random adjective and beverage name
    static const std::vector<std::string> adjectives = {
      "Happy", "Bold", "Chill", "Sunny", "Lucky", "Brave", "Clever", "Calm", "Swift", "Bright"
    };

    const std::vector<Beverage *> &beverages = RecipeDatabase::getAllDefaultRecipes();
    std::string name;
    std::string surname;

    
    size_t maxCombinations = adjectives.size() * beverages.size();
    size_t usedCombinations = m_user_list.size();

    if (!beverages.empty() && usedCombinations < maxCombinations) {
        static bool seeded = false;
        if (!seeded) {
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            seeded = true;
        }
        bool unique = false;
        int attempts = 0;
        do {
            int adjIdx = std::rand() % adjectives.size();
            int bevIdx = std::rand() % beverages.size();
            name = adjectives[adjIdx];
            surname = beverages[bevIdx]->name.value();

            unique = true;
            for (const auto &user : m_user_list) {
                if (user->name.value() == name && user->surname.value() == surname) {
                    unique = false;
                    break;
                }
            }
            attempts++;
        } while (!unique && attempts < 1000); // Prevent infinite loop

        // If after 1000 attempts we didn't find a unique one, fallback
        if (!unique) {
            name = "User";
            surname = std::to_string(id);
        }
    } else {
        name = "User";
        surname = std::to_string(id);
    }
    int picture = 0;

    // Create the new User object
    User *u = new User(name, surname, picture, RecipeDatabase::getAllDefaultRecipes());

    Qul::PlatformInterface::log("[ECM] Enrolled new user %s (id=%d)\n", name.c_str(), id);
    m_user_list.push_back(u);
    m_users_by_id[id] = u;
    EdgeCoffeeMachine::instance().setUser(u);

    // Push to the UI model
    updateUserModel();
  }

  /**
   * @brief Identifies an existing user by ID.
   *
   * Looks up the user in the internal map and sets them as the current user.
   * If the ID is not found, it logs a warning and does not change the current user.
   *
   * @param id The unique ID detected by the AI subsystem.
   */
  void EdgeCoffeeMachine::identifyUser(int id)
  {
    auto it = m_users_by_id.find(id);
    if (it != m_users_by_id.end())
    {
      Qul::PlatformInterface::log("[ECM] Identified user id=%d\n", id);
      EdgeCoffeeMachine::instance().setUser(it->second);
    }
    else
    {
      Qul::PlatformInterface::log("[ECM] Warning: User id=%d not found during identification.\n", id);
    }
  }

  /**
   * @brief Logs out the current user, reverting to Guest mode.
   *
   * This method clears the current user session by setting the user
   * property to nullptr. It also updates the beverage model to switch
   * back to the global popularity list and logs the logout action.
   */
  void EdgeCoffeeMachine::logoutUser()
  {
    setUser(nullptr);
  }
}
