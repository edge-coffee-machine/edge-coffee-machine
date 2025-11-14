#include "User.h"
#include <QDebug>

// Constructor, receives user name, picture index and optional parent QObject
User::User(const QString& name, int picture, QObject* parent)
    : QObject(parent), m_name(name), m_picture(picture), m_category(UserCategory::Default)
{
    //m_beverages = Beverage::createDefaultTemplates(this);
    m_beverages.clear();
    m_beverages.push_back(new Beverage(QStringLiteral("Espresso"), this));
    m_beverages.push_back(new Beverage(QStringLiteral("Cappuccino"), this));
    m_beverages.push_back(new Beverage(QStringLiteral("Americano"), this));

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

    qInfo() << "User::beverageSelected called for beverage:" << beverage->name();

    // Update tryer score
    m_tryerScore = tryerR*(1-m_beveragesW[idx]) + (1-tryerR)*(m_tryerScore);

    // Update customizer score
    if (m_customized) {
        m_customizerScore = customizerR + (1-customizerR)*(m_customizerScore);
    }
    else {
        m_customizerScore = (1-customizerR)*(m_customizerScore);
    }

    m_customized = false;

    // print tryer score, customizer score, and beverage weights for debugging
    qInfo() << "User::beverageSelected: tryerScore =" << m_tryerScore << ", customizerScore =" << m_customizerScore;
    qInfo() << "Beverage weights:";
    for (int i = 0; i < m_beveragesW.size(); i++) {
        qInfo() << "  Beverage" << i << ": weight =" << m_beveragesW[i];
    }

    if (m_numBeverages > 3) { // Classify the user after 3 selections
        float tryerRatio = 0.7f; // Weight of tryer score in adoption score

        // Calculate an early adopter score to classify the user based on the tryer and customizer scores
        float earlyAdopterScore = tryerRatio*m_tryerScore + (1-tryerRatio)*m_customizerScore; 

        qInfo() << "User::beverageSelected: earlyAdopterScore =" << earlyAdopterScore;
        
        // Classify user
        if (earlyAdopterScore < 0.5f) {
            m_category = UserCategory::Conservative;
            m_weightR = conservativeWeightR;
        }
        else {
            m_category = UserCategory::EarlyAdopter;
            m_weightR = earlyAdopterWeightR;
        }
    }

    // Update coffee weights
    for (int i = 0; i < m_beveragesW.size(); i++) {
        if (i == idx) {
            m_beveragesW[i] = m_weightR + (1.0f - m_weightR) * m_beveragesW[i];
        } 
        else {
            m_beveragesW[i] = (1.0f - m_weightR) * m_beveragesW[i];
        }
    }

    // Normalize weights
    float sum = 0.0f;
    for (size_t i = 0; i < m_beveragesW.size(); ++i) {
        sum += m_beveragesW[i];
    }
    if (sum > 0.0f) {
        for (size_t i = 0; i < m_beveragesW.size(); ++i) {
            m_beveragesW[i] /= sum;
        }
    }

    m_numBeverages++;
}

void User::beverageCustomized()
{
    qInfo() << "User::beverage_customized called.";
    m_customized = true;
}