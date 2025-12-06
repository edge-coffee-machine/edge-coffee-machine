#ifndef BEVERAGELISTMODEL_H
#define BEVERAGELISTMODEL_H

#include "Beverage.h"

#include <qul/object.h>
#include <qul/model.h>
#include <qul/singleton.h>

#include <vector>

namespace Logic
{
    /**
     * @class BeverageModel
     * @brief A singleton ListModel that exposes the list of beverages to the QML UI.
     *
     * This class acts as a bridge (View Model) between the backend logic (EdgeCoffeeMachine)
     * and the frontend QML `ListView`. It wraps a standard `std::vector` of Beverage pointers
     * into a `Qul::ListModel` compatible with Qt for MCUs.
     *
     * Being a Singleton, it can be accessed directly in QML files (e.g., `model: Logic.BeverageModel`)
     * without needing to pass pointers manually.
     */
    class BeverageModel : public Qul::ListModel<Beverage *>, public Qul::Singleton<BeverageModel>
    {
        friend class Qul::Singleton<BeverageModel>;

    public:
        /**
         * @brief Returns the number of items in the model.
         *
         * Required override for `Qul::ListModel`.
         * @return The size of the internal data vector.
         */
        int count() const override
        {
            return static_cast<int>(m_data.size());
        }

        /**
         * @brief Retrieves the data item at the specified index.
         *
         * Required override for `Qul::ListModel`. Used by the QML engine to populate
         * the delegates in the ListView.
         *
         * @param index The index of the item to retrieve.
         * @return A pointer to the `Beverage` object, or `nullptr` if the index is out of bounds.
         */
        Beverage *data(int index) const override
        {
            if (index < 0 || index >= static_cast<int>(m_data.size()))
            {
                return nullptr;
            }
            return m_data[index];
        }

        /**
         * @brief Updates the model with a new list of beverages and refreshes the UI.
         *
         * This method is called by the Controller (EdgeCoffeeMachine) whenever the list
         * of beverages changes (e.g., switching between users,
         * or reordering based on popularity).
         *
         * It updates the internal data and emits `modelReset()` to force the QML ListView
         * to redraw completely.
         *
         * @param newList The new vector of Beverage pointers to display.
         */
        void updateList(const std::vector<Beverage *> &newList)
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
        BeverageModel() {}

        /**
         * @brief Internal storage for the list of beverages currently displayed.
         */
        std::vector<Beverage *> m_data;
    };

}

#endif // BEVERAGEMODEL_H
