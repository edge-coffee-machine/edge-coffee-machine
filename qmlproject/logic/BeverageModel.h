#ifndef BEVERAGEMODEL_H
#define BEVERAGEMODEL_H

#include "Beverage.h"

#include <qul/object.h>
#include <qul/model.h>

#include <vector>

namespace Logic
{
    /**
     * @class BeverageModel
     * @brief A ListModel that exposes a collection of beverages to the QML UI.
     *
     * This class acts as a bridge (View Model) between the backend data (std::vector)
     * and the frontend QML `ListView`. It wraps the raw list of `Beverage` pointers
     * into a `Qul::ListModel` compatible with Qt for MCUs.
     *
     * @note It is owned and instantiated by the
     * `EdgeCoffeeMachine` controller. In QML, it is accessed via the
     * `EdgeCoffeeMachine.drinksList` property.
     */
    class BeverageModel : public Qul::ListModel<Beverage *>
    {
    public:
        /**
         * @brief Default constructor.
         *
         * Initializes an empty model. The data is populated later via `updateList`.
         */
        BeverageModel() {};

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
         * @brief Internal storage for the list of beverages currently displayed.
         */
        std::vector<Beverage *> m_data;
    };

}

#endif // BEVERAGEMODEL_H
