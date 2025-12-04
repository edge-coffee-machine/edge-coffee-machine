#ifndef BEVERAGELISTMODEL_H
#define BEVERAGELISTMODEL_H
#include <vector>
#include <Qul/Object.h>
#include <Qul/ListModel.h>
#include <Qul/Singleton.h>
#include "Beverage.h"

namespace Logic
{
    class BeverageModel : public Qul::ListModel<Beverage *>, public Qul::Singleton<BeverageModel>
    {
        friend class Qul::Singleton<BeverageModel>;

    public:
        int count() const override
        {
            return static_cast<int>(m_data.size());
        }
        Beverage *data(int index) const override
        {
            if (index < 0 || index >= static_cast<int>(m_data.size()))
            {
                return nullptr;
            }
            return m_data[index];
        }

        void updateList(const std::vector<Beverage *> &newList)
        {
            if (m_data == newList)
                return;

            beginResetModel();

            m_data = newList;

            endResetModel();
        }

    private:
        BeverageModel() {}

        std::vector<Beverage *> m_data;
    };

}

#endif // BEVERAGEMODEL_H
