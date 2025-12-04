#ifndef USER_H
#define USER_H

#include <Qul/Object.h>
#include <Qul/Property.h>
#include <string>
#include <vector>

#include "Beverage.h"
#include "WeightedSortedList.h"

namespace Logic
{
    /**
     * @class User
     * @brief Represents an individual user profile with personalized preferences and behavior tracking.
     *
     * This class encapsulates the identity and beverage preferences of a specific user.
     * It implements a recommendation system that adapts to the user's choices over time.
     *
     * ### Recommendation & Classification Model
     * - **Weights:** Beverage weights are updated using an exponential decay model. Selecting a beverage increases its weight, while others decay.
     * - **User Categories:** Users are classified into three categories:
     * - **Default:** New users with insufficient data. They follow global popularity trends.
     * - **Conservative:** Users who stick to a few favorites. Weights update slowly.
     * - **Early Adopter:** Users who frequently try new or customized drinks. Weights update quickly, and the UI injects variety.
     * - **Scoring:** The system tracks `tryerScore` (variety seeking) and `customizerScore` (customization frequency) to determine the user category.
     */
    class User : public Qul::Object
    {
    public:
        /**
         * @brief Enumeration of possible user classifications.
         */
        enum class UserCategory
        {
            Default,
            Conservative,
            EarlyAdopter
        };

        /**
         * @brief Constructs a new User with a cloned set of recipes.
         *
         * @param nameVal The display name of the user.
         * @param pictureVal The ID of the user's profile picture.
         * @param personalRecipes A vector of Beverage pointers that this User will own and manage independently.
         */
        User(const std::string &nameVal, int pictureVal, const std::vector<Beverage *> &personalRecipes);

        /**
         * @brief Destructor. Cleans up the personal beverage instances.
         */
        ~User();

        /**
         * @brief The user's display name.
         */
        Qul::Property<std::string> name;

        /**
         * @brief The index of the user's profile picture.
         */
        Qul::Property<int> picture;

        /**
         * @brief Records a beverage selection and updates the recommendation model.
         *
         * This method is called when a drink is brewed. It:
         * 1. Updates the `tryerScore` based on the novelty of the selection.
         * 2. Re-classifies the user (Default -> Conservative/EarlyAdopter).
         * 3. Adjusts the learning rate (`weightR`) based on the category.
         * 4. Updates beverage weights using exponential decay.
         * 5. Refreshes the display list.
         *
         * @param beverage Pointer to the selected beverage.
         */
        void beverageBrewed(Beverage *beverage);

        /**
         * @brief Flags the current session as "Customized".
         *
         * Called when the user modifies ingredients. This boosts the `customizerScore`
         * during the next `beverageBrewed` event.
         */
        void beverageCustomized();

        /**
         * @brief Retrieves the sorted list of beverages for display.
         *
         * The list order depends on the user category:
         * - **Default:** Mirrors global popularity.
         * - **Conservative:** Sorted strictly by personal weight (favorites first).
         * - **Early Adopter:** Sorted by weight, but injects a low-frequency suggestion at the 3rd position.
         *
         * @return A const reference to the vector of beverages.
         */
        const std::vector<Beverage *> &getDisplayBeverages() const;

        /**
         * @brief Gets the current classification category of the user.
         * @return The current UserCategory.
         */
        UserCategory category() const { return m_category; }

    private:
        // Classification thresholds and rates
        static constexpr float defaultWeightR = 0.175f;
        static constexpr float conservativeWeightR = 0.1f;
        static constexpr float earlyAdopterWeightR = 0.25f;
        static constexpr float tryerR = 0.25f;
        static constexpr float customizerR = 0.225f;
        static constexpr float tryerToCustomizerRatio = 0.8f;
        static constexpr float earlyAdopterThreshold = 0.5f;

        UserCategory m_category = UserCategory::Default;
        WeightedSortedList<Beverage *> m_weightedBeverages;
        std::vector<Beverage *> m_displayBeverages;

        int m_numBeverages = 0;
        float m_tryerScore = 0.0f;
        float m_customizerScore = 0.0f;
        bool m_customized = false;

        /**
         * @brief Internal helper to update user scores and category.
         * @param selectedIdx Index of the selected beverage in the weighted list.
         */
        void classifyUser(int selectedIdx);

        /**
         * @brief Internal helper to regenerate the `m_displayBeverages` vector.
         */
        void updateDisplayBeverages();
    };
}

#endif // USER_H