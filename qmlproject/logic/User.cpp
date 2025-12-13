/**
 * @file User.cpp
 * @brief Implementation of the User profile and recommendation logic.
 *
 * This file handles the user-specific logic, including:
 * - Managing a personal list of weighted beverages.
 * - Classifying the user behavior (Early Adopter vs Conservative).
 * - Generating the specific display order of drinks based on the user category.
 */

#include "User.h"
#include "EdgeCoffeeMachine.h"

#include <platforminterface/log.h>

#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace Logic
{
    /**
     * @brief Constructs a new User object.
     *
     * Initializes the user identity and sets up the internal weighted list
     * using a **cloned** set of recipes. This ensures that weight updates
     * for this user do not affect the global recipes or other users.
     *
     * @param nameVal The display name of the user.
     * @param pictureVal The ID of the user's profile picture.
     * @param personalRecipes A vector of Beverage pointers exclusively owned by this User.
     */
    User::User(const std::string &nameVal, int pictureVal, const std::vector<Beverage *> &personalRecipes)
        : m_weightedBeverages(personalRecipes, defaultWeightR)
    {
        name.setValue(nameVal);
        initials.setValue(nameVal.substr(0, 2));
        picture.setValue(pictureVal);        

        m_category = UserCategory::Default;

        m_numBeverages = 0;
        m_tryerScore = 0.0f;
        m_customizerScore = 0.0f;
        m_customized = false;

        updateDisplayBeverages();

        Qul::PlatformInterface::log("[User] Created user: %s (Category: Default)\n", nameVal.c_str());
    }

    /**
     * @brief Destructor.
     *
     * Responsible for deleting the `Beverage` objects in `m_weightedBeverages`,
     * as this class owns the cloned instances passed in the constructor.
     */
    User::~User()
    {
        for (Beverage *b : m_weightedBeverages.items())
        {
            delete b;
        }
    }

    /**
     * @brief Retrieves the pre-calculated list of beverages to display.
     *
     * @return A const reference to the sorted vector of beverages.
     */
    const std::vector<Beverage *> &User::getDisplayBeverages() const
    {
        return m_displayBeverages;
    }

    /**
     * @brief Regenerates the `m_displayBeverages` vector based on User Category.
     *
     * This method implements the core recommendation presentation logic:
     * - **Default User**: Ignores personal weights. Instead, it mirrors the **Global Popularity**
     * order (fetched from `EdgeCoffeeMachine`), mapping the global names to the user's local instances.
     * - **Conservative User**: Shows the list sorted strictly by personal weights (favorites first).
     * - **Early Adopter**: Shows the list sorted by weight, but **injects a "suggestion"**
     * (a low-frequency drink) into the 3rd position (index 2) to encourage variety.
     */
    void User::updateDisplayBeverages()
    {
        m_displayBeverages.clear();

        if (m_category == UserCategory::Default)
        {
            const std::vector<Beverage *> &globalList = EdgeCoffeeMachine::instance().getPopularBeverages();
            const std::vector<Beverage *> &myItems = m_weightedBeverages.items();

            for (const Beverage *globalBev : globalList)
            {
                std::string targetName = globalBev->name.value();

                for (Beverage *myBev : myItems)
                {
                    if (myBev->name.value() == targetName)
                    {
                        m_displayBeverages.push_back(myBev);
                        break;
                    }
                }
            }

            for (Beverage *myBev : myItems)
            {
                auto it = std::find(m_displayBeverages.begin(), m_displayBeverages.end(), myBev);
                if (it == m_displayBeverages.end())
                {
                    m_displayBeverages.push_back(myBev);
                }
            }
        }
        else
        {
            std::vector<Beverage *> sorted = m_weightedBeverages.items();

            if (m_category == UserCategory::EarlyAdopter && sorted.size() >= 4)
            {
                Beverage *suggestion = sorted.back();
                sorted.pop_back();
                sorted.insert(sorted.begin() + 2, suggestion);
            }

            m_displayBeverages = sorted;
        }
    }

    /**
     * @brief Handles the event of a beverage being brewed.
     *
     * This is the main "learning" trigger. It performs the following steps:
     * 1. **Classify**: Updates user scores and Category.
     * 2. **Update Weights**: Applies exponential decay to increase the selected drink's weight.
     * 3. **Update View**: Regenerates the display list.
     *
     * @param beverage Pointer to the brewed beverage.
     */
    void User::beverageBrewed(Beverage *beverage)
    {
        if (!beverage)
            return;

        Qul::PlatformInterface::log("[User] beverageBrewed called for: %s\n", 
                                        beverage->name.value().c_str());

        int idx = m_weightedBeverages.indexOf(beverage);

        if (idx < 0)
        {
            Qul::PlatformInterface::log("[User] Error: Beverage not found in user list!\n");
            return;
        }
        
        m_numBeverages++;
        classifyUser(idx);

        m_weightedBeverages.recordSelectionAt(idx);

        Qul::PlatformInterface::log("[User] Beverage weights after selection:\n");
        const std::vector<Beverage *> &items = m_weightedBeverages.items();

        for (int i = 0; i < m_weightedBeverages.size(); i++)
        {
            Qul::PlatformInterface::log("      [%d] %s : weight = %f\n",
                                        i,
                                        items[i]->name.value().c_str(),
                                        static_cast<double>(m_weightedBeverages.weightAt(i)));
        }

        updateDisplayBeverages();
        m_customized = false;
    }

    /**
     * @brief Marks the current selection session as "Customized".
     *
     * Called when the user modifies ingredients. This affects the `m_customizerScore`
     * during the next `beverageBrewed` call.
     */
    void User::beverageCustomized()
    {
        Qul::PlatformInterface::log("[User] beverageCustomized called.\n");
        m_customized = true;
    }

    /**
     * @brief Updates behavioral scores and determines the User Category.
     *
     * Calculates two metrics:
     * - **Tryer Score**: Increases when the user selects low-weight (novel) beverages.
     * - **Customizer Score**: Increases when `m_customized` is true.
     *
     * These are combined into an `earlyAdopterScore`. If the score exceeds `earlyAdopterThreshold`
     * (and sufficient data exists), the user becomes an **Early Adopter**. Otherwise,
     * they are **Conservative** or **Default**.
     *
     * @param selectedIdx Index of the selected beverage in the weighted list.
     */
    void User::classifyUser(int selectedIdx)
    {
        // Update tryer score
        float w = m_weightedBeverages.weightAt(selectedIdx);
        m_tryerScore = tryerR * (1 - w) + (1 - tryerR) * (m_tryerScore);

        // Update customizer score
        if (m_customized)
        {
            m_customizerScore = customizerR + (1 - customizerR) * (m_customizerScore);
        }
        else
        {
            m_customizerScore = (1 - customizerR) * (m_customizerScore);
        }

        m_customized = false;

        // print tryer score, customizer score, and beverage weights for debugging
        Qul::PlatformInterface::log("[User] Scores updated: Tryer=%.2f, Customizer=%.2f\n",
                                    static_cast<double>(m_tryerScore),
                                    static_cast<double>(m_customizerScore));

        if (m_numBeverages >= 3)
        { // Classify the user after 3 selections
            // Calculate an early adopter score to classify the user based on the tryer and customizer scores
            float earlyAdopterScore = tryerToCustomizerRatio * m_tryerScore + (1 - tryerToCustomizerRatio) * m_customizerScore;

            Qul::PlatformInterface::log("[User] EarlyAdopterScore: %.2f\n", static_cast<double>(earlyAdopterScore));

            // Classify user
            if (earlyAdopterScore < earlyAdopterThreshold)
            {
                m_category = UserCategory::Conservative;
                m_weightedBeverages.setWeightR(conservativeWeightR);
            }
            else
            {
                m_category = UserCategory::EarlyAdopter;
                m_weightedBeverages.setWeightR(earlyAdopterWeightR);
            }
        }

        const char *categoryStr = "Unknown";
        switch (m_category)
        {
        case UserCategory::Default:
            categoryStr = "Default";
            break;
        case UserCategory::Conservative:
            categoryStr = "Conservative";
            break;
        case UserCategory::EarlyAdopter:
            categoryStr = "Early Adopter";
            break;
        }

        Qul::PlatformInterface::log("[User] Current Category: %s\n", categoryStr);
    }
}
