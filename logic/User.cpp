#include "User.h"
#include <QDebug>

// Constructor, receives user name, picture index and optional parent QObject
User::User(const QString& name, int picture, QObject* parent)
    : QObject(parent), m_name(name), m_picture(picture), m_category(UserCategory::Default)
{
    m_weightedBeverages = WeightedSortedList<Beverage*>(Beverage::getIndependentBeverageList(), defaultWeightR);
    updateDisplayBeverages();
    emit displayBeveragesChanged();
}

QString User::name() const { return m_name; }
int User::picture() const { return m_picture; }

void User::test()
{
    qInfo() << "User::test called.";

    //Select beverages
    beverageBrewed(m_weightedBeverages.items()[0]);
    beverageBrewed(m_weightedBeverages.items()[2]);
    beverageBrewed(m_weightedBeverages.items()[4]);
    beverageBrewed(m_weightedBeverages.items()[2]);
    beverageBrewed(m_weightedBeverages.items()[3]);
    beverageBrewed(m_weightedBeverages.items()[1]);
}

// TODO: For default users, it should return the list of beverages by global popularity (populatiy list in EdgeCoffeeMachine, weightedBeverages.items())
QQmlListProperty<Beverage> User::displayBeverages()
{
    return QQmlListProperty<Beverage>(this, &m_displayBeverages);
}

void User::beverageBrewed(Beverage* beverage)
{
    if (!beverage) return;

    qInfo() << "User::beverageBrewed called for beverage:" << beverage->name();

    // Locate the beverage index
    int idx = m_weightedBeverages.indexOf(beverage);

    if (idx < 0) {
        qWarning() << "   User::beverageBrewed: Beverage not found!";
        return;
    }

    classifyUser(idx);
    m_weightedBeverages.recordSelectionAt(idx);
    updateDisplayBeverages();

    //print the beverage weights for debugging
    qInfo() << "   Beverage weights after selection:";
    for (int i = 0; i < m_weightedBeverages.size(); i++) {
        qInfo() << "      Beverage [" << i << "]: " << m_weightedBeverages.items()[i]->name()
                << ", weight =" << m_weightedBeverages.weightAt(i);
    }

    m_numBeverages++;
}

void User::beverageCustomized()
{
    qInfo() << "User::beverage_customized called.";
    m_customized = true;
}

void User::classifyUser(int selectedIdx){
    // Update tryer score
    float w = m_weightedBeverages.weightAt(selectedIdx);
    m_tryerScore = tryerR*(1-w) + (1-tryerR)*(m_tryerScore);

    // Update customizer score
    if (m_customized) {
        m_customizerScore = customizerR + (1-customizerR)*(m_customizerScore);
    }
    else {
        m_customizerScore = (1-customizerR)*(m_customizerScore);
    }

    m_customized = false;

    // print tryer score, customizer score, and beverage weights for debugging
    qInfo() << "   User::beverageBrewed: tryerScore =" << m_tryerScore << ", customizerScore =" << m_customizerScore;

    if (m_numBeverages >= 3) { // Classify the user after 3 selections
        // Calculate an early adopter score to classify the user based on the tryer and customizer scores
        float earlyAdopterScore = tryerToCustomizerRatio*m_tryerScore + (1-tryerToCustomizerRatio)*m_customizerScore; 

        qInfo() << "   User::beverageBrewed: earlyAdopterScore =" << earlyAdopterScore;
        
        // Classify user
        if (earlyAdopterScore < earlyAdopterTreshold) {
            m_category = UserCategory::Conservative;
            m_weightedBeverages.setWeightR(conservativeWeightR);
        }
        else {
            m_category = UserCategory::EarlyAdopter;
            m_weightedBeverages.setWeightR(earlyAdopterWeightR);
        }
    }

    QString categoryStr;
    switch (m_category) {
        case UserCategory::Default:
            categoryStr = QStringLiteral("Default");
            break;
        case UserCategory::Conservative:
            categoryStr = QStringLiteral("Conservative");
            break;
        case UserCategory::EarlyAdopter:
            categoryStr = QStringLiteral("Early adopter");
            break;
        default:
            categoryStr = QStringLiteral("Unknown");
            break;
    }

    qInfo() << "   User::beverageBrewed: User category =" << categoryStr;
}

void User::updateDisplayBeverages()
{
    m_displayBeverages = m_weightedBeverages.items();
    int n = m_displayBeverages.size();

    if (m_category == UserCategory::EarlyAdopter && n > 3)
    {
        int randomIdx = n-1 - (rand() % std::min(3, n-1)); // Select a random index among the three less frequent beverages

        Beverage* suggestion = m_displayBeverages[randomIdx];

        // Remove the suggestion from its current position
        m_displayBeverages.removeAll(suggestion);

        // Insert the suggestion at index 2
        m_displayBeverages.insert(2, suggestion);
    }

    qInfo() << "   Display beverages updated:";
    for (int i = 0; i < m_displayBeverages.size(); i++) {
        qInfo() << "      Display beverage [" << i << "]: " << m_displayBeverages[i]->name();
    }

    emit displayBeveragesChanged();
}
