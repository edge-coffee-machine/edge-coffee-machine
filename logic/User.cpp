#include "User.h"
#include <QDebug>

// Constructor, receives user name, picture index and optional parent QObject
User::User(const QString& name, int picture, QObject* parent)
    : QObject(parent), m_name(name), m_picture(picture), m_category(UserCategory::Default)
{
    //m_beverages = Beverage::createDefaultTemplates(this);
    m_beveragesW = std::vector<float>(m_beverages.size(), 0.0f);
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

    // Update tryer score
    m_tryerScore = tryerR*(1-m_beveragesW[idx]) + (1-tryerR)*(m_tryerScore);

    // Update customizer score
    if (m_customizations == 0) {
        m_customizerScore = (1-customizerR)*(m_customizerScore);
    }
    else {
        for (int i = 0; i < m_customizations; i++){
            m_customizerScore = customizerR + (1-customizerR)*(m_customizerScore);
        }
    }

    m_customizations = 0;

    // Update coffee weights
    for (int i = 0; i < m_beveragesW.size(); i++) {
        if (i == idx) {
            m_beveragesW[i] = m_weightR + (1.0f - m_weightR) * m_beveragesW[i];
        } 
        else {
            m_beveragesW[i] = (1.0f - m_weightR) * m_beveragesW[i];
        }
    }
}

void User::beverageCustomized(Beverage* beverage, float coffee, float water, float cocoa, float milk, float foam)
{
    if (!beverage) {
        qWarning() << "User::beverage_customized: null beverage";
        return;
    }

    qInfo() << "User::beverage_customized called (not implemented yet)."
            << "params: coffee=" << coffee << "water=" << water
            << "cocoa=" << cocoa << "milk=" << milk << "foam=" << foam;

    m_customizations++;
}
