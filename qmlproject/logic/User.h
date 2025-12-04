#ifndef USER_H
#define USER_H

#include <Qul/Object.h>
#include <Qul/Property.h>
#include <string>
#include <vector>

#include "Beverage.h"
#include "WeightedSortedList.h"

/*
 * User
 *
 * Purpose
 * -------
 * Represents a person of the coffee machine application, exposing user identity and
 * personalized beverage preferences to QML and the recommendation subsystem.
 * The class encapsulates a small recommendation model that updates per-beverage
 * weights and user behaviour scores (tryer / customizer) whenever the user
 * selects (brews) or customizes a beverage from the UI.
 * 
 * Implementation notes
 * --------------------
 * - The beverages method exposes m_beverages to QML as a QQmlListProperty. m_beverages is always
 *   sorted by weight after updates to ensure the UI reflects current recommendations.
 * - m_beverages and m_beveragesW are kept in sync via parallel updates and sorting.
 *
 * Recommendation & Classification Model (high level)
 * -------------------------------------------------
 * - Per-beverage weights are updated using an exponential-style decay model:
 *     - The selected beverage receives a positive update (increased weight).
 *     - Other beverages are decayed accordingly so the vector remains meaningful
 *       for ranking and recommendation.
 * - The per-user weight update rate m_weightR is driven by the user's current
 *   category (Default, Conservative, EarlyAdopter). Category-specific constants
 *   (conservativeWeightR, earlyAdopterWeightR) determine how aggressively weights
 *   move after a selection.
 * - Two behavioural scores are tracked:
 *     - m_tryerScore: how often the user tries new beverages
 *     - m_customizerScore: how often the user customizes beverages before selecting
 *   These are updated when selections occur.
 * - classifyUser() combines tryer and customizer scores (using tryerToCustomizerRatio)
 *   and a threshold (earlyAdopterTreshold) to promote users into the EarlyAdopter
 *   category. Classification affects future weight update rates.
 */

namespace Logic
{
    class User : public Qul::Object
    {
    public:
        enum class UserCategory
        {
            Default,
            Conservative,
            EarlyAdopter
        };

        User(const std::string &nameVal, int pictureVal, const std::vector<Beverage *> &personalRecipes);

        ~User();

        Qul::Property<std::string> name;
        Qul::Property<int> picture;
        
         /*
        Called from edge coffee machine when a beverage is brewed
        beverage: pointer to the beverage selected by the user from QML

        Updates the weights of the beverages according to the user's category and selection.
        Follows an exponential decay model where the selected beverage's weight is increased
        and the others are decreased, based on the user's update rate m_r.
        */
        void beverageBrewed(Beverage *beverage);

        /*
        Called from edge coffee machine when a beverage is customized
        beverage: pointer to the beverage customized by the user from QML

        Records that the user has customized a beverage for recommendation purposes.
        For the actual change in ingredients, use the Beverage methods directly.
        */
        void beverageCustomized();

        /*
        Gets the list of user's beverages to display for QML access.
        For conservative users, the list is sorted by recommendation weight, so that 
        more recommended beverages appear earlier in the list.
        For early adopters, the third beverage in the list is always an unfrequent beverage.
        */
        const std::vector<Beverage *> &getDisplayBeverages() const;

        UserCategory category() const { return m_category; }

    private:
        static constexpr float defaultWeightR = 0.175f;
        static constexpr float conservativeWeightR = 0.1f;
        static constexpr float earlyAdopterWeightR = 0.25f;
        static constexpr float tryerR = 0.25f;
        static constexpr float customizerR = 0.225f;
        static constexpr float tryerToCustomizerRatio = 0.8f;
        static constexpr float earlyAdopterThreshold = 0.5f;

        UserCategory m_category = UserCategory::Default;
        WeightedSortedList<Beverage *> m_weightedBeverages;
        std::vector<Beverage*> m_displayBeverages;
        
        int m_numBeverages = 0;
        float m_tryerScore = 0.0f;
        float m_customizerScore = 0.0f;
        bool m_customized = false;

        /*
        Should only be called from beverageBrewed.
        Updates the user scores and category based on the selected beverage.
        Resets m_customized flag.
        selectedIdx: index of the beverage that has been selected in m_beverages
        */
        void classifyUser(int selectedIdx);

        /*
        Updates m_displayBeverages based on the current user category and beverage list.
        */
        void updateDisplayBeverages();
    };
}

#endif