#include "User.h"
#include <QDebug>

// Constructor, receives user name, picture index and optional parent QObject
User::User(const QString& name, int picture, QObject* parent)
    : QObject(parent), m_name(name), m_picture(picture), m_category(UserCategory::Default)
{
    //m_beverages = Beverage::createDefaultTemplates(this);
    m_beverages_w = QVector<float>(static_cast<int>(m_beverages.size()), 0.0f);
    emit beveragesChanged();
}

QString User::name() const { return m_name; }
int User::picture() const { return m_picture; }

QQmlListProperty<Beverage> User::beverages()
{
    return QQmlListProperty<Beverage>(this, this,
        [](QQmlListProperty<Beverage>* list) -> qsizetype {
            return static_cast<qsizetype>(reinterpret_cast<User*>(list->data)->m_beverages.size());
        },
        [](QQmlListProperty<Beverage>* list, qsizetype index) -> Beverage* {
            return reinterpret_cast<User*>(list->data)->m_beverages.at(static_cast<size_t>(index));
        }
    );
}

/*
beverage: pointer to the beverage selected by the user from QML

Updates the weights of the beverages according to the user's category and selection.
Follows an exponential decay model where the selected beverage's weight is increased
and the others are decreased, based on the user's update rate m_r.
*/
void User::beverageSelected(Beverage* beverage)
{
    if (!beverage) return;

    // Locate the beverage index
    int idx = -1;
    for (int i = 0; i < m_beverages.size(); i++) {
        if (m_beverages[i] == beverage) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        qWarning() << "User::beverageSelected: Beverage not found!";
        return;
    }

    // Update weights
    for (int i = 0; i < m_beverages_w.size(); i++) {
        float w = m_beverages_w.at(i);

        if (i == idx) {
            w = m_r + (1.0f - m_r) * w;
        } 
        else {
            w = (1.0f - m_r) * w;
        }

        m_beverages_w[i] = w;
    }
}
