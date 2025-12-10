#ifndef EDGE_COFFEE_MACHINE_H
#define EDGE_COFFEE_MACHINE_H

#include "Beverage.h"
#include "User.h"
#include "WeightedSortedList.h"
#include "RecipeDatabase.h"

#include <qul/object.h>
#include <qul/singleton.h>
#include <qul/property.h>
#include <qul/timer.h>

#include <vector>
#include <string>
#include <map>

namespace Logic
{
    /**
     * @class EdgeCoffeeMachine
     * @brief The central controller and singleton for the Coffee Machine application.
     *
     * This class acts as the main backend logic entry point. It manages:
     * - The global machine state (Idle, Brewing, Selected Drink).
     * - The current user session (Guest vs Logged-in User).
     * - The simulation of the brewing process via a timer.
     * - The synchronization between raw data and the UI View Models (`BeverageListModel`).
     *
     * Being a `Qul::Singleton`, it is accessible globally in QML as `EdgeCoffeeMachine`.
     */
    class EdgeCoffeeMachine : public Qul::Singleton<EdgeCoffeeMachine>
    {
        friend class Qul::Singleton<EdgeCoffeeMachine>;

    public:
        /**
         * @brief Current textual status of the machine.
         *
         * Examples: "Idle", "Making Cappuccino...", "Welcome Matteo".
         * Used for UI feedback labels.
         */
        Qul::Property<std::string> status;

        /**
         * @brief Flag indicating if a brewing process is currently active.
         *
         * Used to disable UI controls (like the Brew button) during operation.
         */
        Qul::Property<bool> isMakingDrink;

        /**
         * @brief Pointer to the currently selected beverage.
         *
         * This can be null if no selection has been made.
         */
        Qul::Property<Beverage *> selectedBeverage;

        /**
         * @brief Pointer to the currently logged-in user.
         *
         * If `nullptr`, the machine is in "Default Mode" and uses global popularity weights.
         * If set, the machine uses the user's personalized preferences.
         */
        Qul::Property<User *> user;

        /**
         * @brief Starts the brewing process.
         *
         * Checks if the machine is idle and a valid beverage is selected.
         * If successful, sets `isMakingDrink` to true and starts the brew timer.
         *
         * @param beverage Optional pointer. If provided, brews this specific beverage.
         * If nullptr, brews the `selectedBeverage`.
         */
        void makeDrink(Beverage *beverage);

        /**
         * @brief Stops the brewing process immediately.
         *
         * Cancels the brew timer and resets the machine state to Idle.
         * If a drink was being brewed, it is considered cancelled.
         */
        void stopBrewing();

        /**
         * @brief Sets the currently selected beverage.
         *
         * Updates the `selectedBeverage` property and the status message.
         * @param beverage Pointer to the beverage to select.
         */
        void selectBeverage(Beverage *beverage);

        /**
         * @brief Logs in a specific user or logs out (if nullptr).
         *
         * Triggers a model update to switch the displayed beverage list from
         * Default to Personalized User.
         * @param user Pointer to the User object or nullptr.
         */
        void setUser(User *user);

        /**
         * @brief Registers a new user ID.
         * @param id The unique ID assigned by the AI subsystem.
         */
        void enrollUser(int id);

        /**
         * @brief Identifies an existing user by ID.
         * @param id The unique ID detected by the AI subsystem.
         */
        void identifyUser(int id);

        /**
         * @brief Logs out the current user, reverting to Guest mode.
         */
        void logoutUser();

        /**
         * @return The currently selected beverage pointer.
         */
        Beverage *getSelectedBeverage() const { return selectedBeverage.value(); }

        /**
         * @brief Retrieves the raw list of beverages sorted by global popularity.
         * @return Const reference to the internal vector.
         */
        const std::vector<Beverage *> &getPopularBeverages() const;

        /**
         * @return True if the machine is currently brewing, false otherwise.
         */
        bool getIsMakingDrink() const { return isMakingDrink.value(); }

        /**
         * @return Pointer to the current user, or nullptr if guest.
         */
        User *getUser() const { return user.value(); }

    private:
        /**
         * @brief Private constructor to enforce Singleton pattern.
         *
         * Initializes the machine state, loads default recipes, and sets up the timer.
         */
        EdgeCoffeeMachine();

        /**
         * @brief Updates the weight/popularity of a beverage after selection.
         *
         * @param drink The beverage that was selected.
         */
        void recordBeverageSelection(Beverage *drink);

        /**
         * @brief Slot called when the brew timer expires.
         *
         * Finalizes the brewing process, updates statistics,
         * and resets the machine state.
         */
        void finishBrewing();

        /**
         * @brief Synchronizes the backend data with the UI View Models.
         *
         * Pushes the correct list of beverages (User's list or Global list)
         * to the `BeverageModel` singleton so the UI updates.
         */
        void updateBeverageModel();

        /**
         * @brief Updates the UserModel with the complete list of registered users.
         *
         * Retrieves the full internal list of enrolled users (m_user_list) and pushes
         * it to the `UserModel` singleton. This ensures that UI components designed for
         * user selection or administration panels have access to the latest, complete set of profiles.
         */
        void updateUserModel();

        /**
         * @brief Central synchronization point for all QML View Models.
         *
         * This function orchestrates the update process whenever the application state changes.
         * It delegates the update by calling both the specialized beverage model helper and
         * the user model helper to ensure complete UI coherence across all list views.
         */
        void updateModels();

        /**
         * @brief Global list of beverages sorted by popularity (Weighted Learning).
         * Used when no user is logged in.
         */
        WeightedSortedList<Beverage *> m_weightedBeverages;

        std::vector<User *> m_user_list;     ///< List of all registered users.
        std::map<int, User *> m_users_by_id; ///< Map for fast user lookup by ID.
        Qul::Timer m_brewTimer;              //< Timer to simulate brewing duration.

        static constexpr float popularityWeightR = 0.175f; ///< Learning rate for global popularity.
    };
}

#endif // EDGE_COFFEE_MACHINE_H
