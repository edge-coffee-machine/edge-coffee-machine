#ifndef USERLISTMODEL_H
#define USERLISTMODEL_H

#include <Qul/ListModel.h>
#include <Qul/Singleton.h>
#include <vector>
#include "User.h"

namespace Logic
{
    class UserModel : public Qul::ListModel<User *>, public Qul::Singleton<UserModel>
    {
        friend class Qul::Singleton<UserModel>;

    public:
        int count() const override
        {
            return static_cast<int>(m_data.size());
        }

        User *data(int index) const override
        {
            if (index < 0 || index >= static_cast<int>(m_data.size()))
            {
                return nullptr;
            }
            return m_data[index];
        }

        void updateList(const std::vector<User *> &newList)
        {
            if (m_data == newList)
                return;

            beginResetModel();
            m_data = newList;
            endResetModel();
        }

    private:
        UserModel() {}

        std::vector<User *> m_data;
    };
}

#endif // USERMODEL_H