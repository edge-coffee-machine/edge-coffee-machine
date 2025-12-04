#ifndef EDGE_COFFEE_MACHINE_H
#define EDGE_COFFEE_MACHINE_H

#include <Qul/Object.h>
#include <Qul/Singleton.h>
#include <Qul/Property.h>
#include <Qul/Timer.h>
#include <vector>
#include <string>
#include <map>

#include "Beverage.h"
#include "User.h"
#include "WeightedSortedList.h"
#include "RecipeDatabase.h"

namespace Logic
{
    class EdgeCoffeeMachine : public Qul::Singleton<EdgeCoffeeMachine>
    {
        friend class Qul::Singleton<EdgeCoffeeMachine>;

    public:
        Qul::Property<std::string> status;
        Qul::Property<bool> isMakingDrink;
        Qul::Property<Beverage *> selectedBeverage;
        Qul::Property<User *> user;

        void makeDrink(Beverage *beverage);
        void selectBeverage(Beverage *beverage);
        void setUser(User *user);
        void enrollUser(int id);
        void identifyUser(int id);

        Beverage *getSelectedBeverage() const { return selectedBeverage.value(); }
        const std::vector<Beverage *> &getPopularBeverages() const;
        bool getIsMakingDrink() const { return isMakingDrink.value(); }
        User *getUser() const { return user.value(); }

    private:
        EdgeCoffeeMachine();

        void recordBeverageSelection(Beverage *drink);
        void finishBrewing();
        void updateModels();

        WeightedSortedList<Beverage *> m_weightedBeverages;
        std::vector<User *> m_user_list;
        std::map<int, User *> m_users_by_id;
        Qul::Timer m_brewTimer;

        static constexpr float popularityWeightR = 0.175f;
    };
}

#endif // EDGE_COFFEE_MACHINE_H
