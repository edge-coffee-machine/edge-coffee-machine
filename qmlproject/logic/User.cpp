#include "User.h"
#include "EdgeCoffeeMachine.h"
#include <Qul/Log.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>

namespace Logic
{
    User::User(const std::string &nameVal, int pictureVal, const std::vector<Beverage *> &personalRecipes)
        : m_weightedBeverages(personalRecipes, defaultWeightR)
    {
        name.setValue(nameVal);
        picture.setValue(pictureVal);

        m_category = UserCategory::Default;

        m_numBeverages = 0;
        m_tryerScore = 0.0f;
        m_customizerScore = 0.0f;
        m_customized = false;

        updateDisplayBeverages();

        Qul::PlatformInterface::log("[User] Created user: %s (Category: Default)\n", nameVal.c_str());
    }

    User::~User()
    {
        for (Beverage *b : m_weightedBeverages.items())
        {
            delete b;
        }
    }

    const std::vector<Beverage *> &User::getDisplayBeverages() const
    {
        return m_displayBeverages;
    }

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

        classifyUser(idx);

        switch (m_category)
        {
        case UserCategory::Conservative:
            m_weightedBeverages.setWeightR(conservativeWeightR);
            break;
        case UserCategory::EarlyAdopter:
            m_weightedBeverages.setWeightR(earlyAdopterWeightR);
            break;
        default:
            m_weightedBeverages.setWeightR(defaultWeightR);
            break;
        }

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

        m_numBeverages++;
        m_customized = false;
    }

    void User::beverageCustomized()
    {
        Qul::PlatformInterface::log("[User] beverageCustomized called.\n");
        m_customized = true;
    }

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

    void User::updateDisplayBeverages()
    {
        m_displayBeverages = m_weightedBeverages.items();
        int n = static_cast<int>(m_displayBeverages.size());

        if (m_category == UserCategory::EarlyAdopter && n > 3)
        {
            // Select a random index among the three less frequent beverages using QRandomGenerator
            int tail = std::min(3, n - 1);
            int randomOffset = std::rand() % tail;
            int randomIdx = n - 1 - randomOffset;

            Beverage *suggestion = m_displayBeverages[randomIdx];

            // Remove the suggestion from its current position
            m_displayBeverages.erase(m_displayBeverages.begin() + randomIdx);

            // Insert the suggestion at index 2
            m_displayBeverages.insert(m_displayBeverages.begin() + 2, suggestion);
        }

        Qul::PlatformInterface::log("[User] Display beverages updated:\n");
        for (size_t i = 0; i < m_displayBeverages.size(); ++i)
        {
            Qul::PlatformInterface::log("      Display beverage [%d]: %s\n",
                                        static_cast<int>(i),
                                        m_displayBeverages[i]->name.value().c_str());
        }
    }
}