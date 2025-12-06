#ifndef USERLISTMODEL_H
#define USERLISTMODEL_H

#include "User.h"

#include <qul/model.h>
#include <qul/singleton.h>

#include <vector>

namespace Logic
{
    /**
     * @class UserModel
     * @brief A singleton ListModel that exposes the list of registered users to the QML UI.
     *
     * This class acts as the interface (View Model) between the backend logic
     * (specifically the user management inside `EdgeCoffeeMachine`) and the frontend QML.
     * It wraps a `std::vector` of `User` pointers into a `Qul::ListModel`, allowing
     * visual elements to display user data.
     *
     * As a Singleton, it can be accessed globally in QML (e.g., `model: Logic.UserModel`).
     */
    class UserModel : public Qul::ListModel<User *>, public Qul::Singleton<UserModel>
    {
        friend class Qul::Singleton<UserModel>;

    public:
        /**
         * @brief Returns the total number of users in the model.
         *
         * Required override for `Qul::ListModel`.
         * @return The size of the internal user vector.
         */
        int count() const override
        {
            return static_cast<int>(m_data.size());
        }

        /**
         * @brief Retrieves the User object at the specified index.
         *
         * Required override for `Qul::ListModel`. Used by the QML engine to populate
         * the delegates in a ListView.
         *
         * @param index The index of the user to retrieve.
         * @return A pointer to the `User` object, or `nullptr` if the index is out of bounds.
         */
        User *data(int index) const override
        {
            if (index < 0 || index >= static_cast<int>(m_data.size()))
            {
                return nullptr;
            }
            return m_data[index];
        }

        /**
         * @brief Updates the model with a new list of users and refreshes the UI.
         *
         * This method is called by the `EdgeCoffeeMachine` controller whenever the
         * list of users changes (e.g., a new user is enrolled/registered).
         * It updates the internal data storage and triggers `modelReset()` to
         * force the UI to redraw the list.
         *
         * @param newList The new vector of User pointers to display.
         */
        void updateList(const std::vector<User *> &newList)
        {
            if (m_data == newList)
                return;

            m_data = newList;

            modelReset();
        }

    private:
        /**
         * @brief Private constructor to enforce the Singleton pattern.
         */
        UserModel() {}

        /**
         * @brief Internal storage for the list of user pointers.
         */
        std::vector<User *> m_data;
    };
}

#endif // USERMODEL_H
