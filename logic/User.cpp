#include "User.h"
#include <QDebug>

// Constructor, receives user name, picture index and optional parent QObject
User::User(const QString& name, int picture, QObject* parent)
    : QObject(parent), m_name(name), m_picture(picture), m_category(UserCategory::Default)
{
    //m_beverages = Beverage::createDefaultTemplates(this);

    float w = 1.0f / static_cast<float>(m_beverages.size());
    m_beveragesW = std::vector<float>(m_beverages.size(), w);
    emit displayBeveragesChanged();
}

QString User::name() const { return m_name; }
int User::picture() const { return m_picture; }

// TODO: For default users, it should return the list of beverages by global popularity
QQmlListProperty<Beverage> User::displayBeverages()
{
    return QQmlListProperty<Beverage>(
        this, &m_displayBeverages,
        [](QQmlListProperty<Beverage>* prop) {
            auto vec = static_cast<std::vector<Beverage*>*>(prop->data);
            return qsizetype(vec->size());
        },
        [](QQmlListProperty<Beverage>* prop, qsizetype index) {
            auto vec = static_cast<std::vector<Beverage*>*>(prop->data);
            return (*vec)[index];
        }
    );
}

void User::beverageSelected(Beverage* beverage)
{
    if (!beverage) return;

    qInfo() << "User::beverageSelected called for beverage:" << beverage->name();

    // Locate the beverage index
    int idx = -1;
    for (int i = 0; i < m_beverages.size(); i++) {
        if (m_beverages[i] == beverage) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        qWarning() << "   User::beverageSelected: Beverage not found!";
        return;
    }

    classifyUser(idx);
    updateBeverageWeights(idx);
    reorderBeverage(idx);

    m_numBeverages++;
}

void User::beverageCustomized()
{
    qInfo() << "User::beverage_customized called.";
    m_customized = true;
}

void User::updateBeverageWeights(int selectedIdx)
{
    // Update coffee weights
    for (int i = 0; i < m_beveragesW.size(); i++) {
        if (i == selectedIdx) {
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
}

void User::classifyUser(int selectedIdx){
    // Update tryer score
    m_tryerScore = tryerR*(1-m_beveragesW[selectedIdx]) + (1-tryerR)*(m_tryerScore);

    // Update customizer score
    if (m_customized) {
        m_customizerScore = customizerR + (1-customizerR)*(m_customizerScore);
    }
    else {
        m_customizerScore = (1-customizerR)*(m_customizerScore);
    }

    m_customized = false;

    // print tryer score, customizer score, and beverage weights for debugging
    qInfo() << "   User::beverageSelected: tryerScore =" << m_tryerScore << ", customizerScore =" << m_customizerScore;

    if (m_numBeverages >= 3) { // Classify the user after 3 selections
        // Calculate an early adopter score to classify the user based on the tryer and customizer scores
        float earlyAdopterScore = tryerToCustomizerRatio*m_tryerScore + (1-tryerToCustomizerRatio)*m_customizerScore; 

        qInfo() << "   User::beverageSelected: earlyAdopterScore =" << earlyAdopterScore;
        
        // Classify user
        if (earlyAdopterScore < earlyAdopterTreshold) {
            m_category = UserCategory::Conservative;
            m_weightR = conservativeWeightR;
        }
        else {
            m_category = UserCategory::EarlyAdopter;
            m_weightR = earlyAdopterWeightR;
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

    qInfo() << "   User::beverageSelected: User category =" << categoryStr;
}

void User::reorderBeverage(int selectedIdx)
{
    int i = selectedIdx;

    // Move upwards
    while (i > 0 && m_beveragesW[i] > m_beveragesW[i - 1]) {
        std::swap(m_beverages[i],  m_beverages[i - 1]);
        std::swap(m_beveragesW[i], m_beveragesW[i - 1]);
        i--;
    }

    qInfo() << "   New beverage weights:";
    for (int i = 0; i < m_beveragesW.size(); i++) {
        qInfo() << "      Beverage" << i << "(" << m_beverages[i]->name() << "): weight =" << m_beveragesW[i];
    }

    updateDisplayBeverages();
}

void User::updateDisplayBeverages()
{
    m_displayBeverages = m_beverages;

    if (m_category == UserCategory::EarlyAdopter && m_beverages.size() > 3)
    {
        int n = m_beverages.size();
        int randomIdx = n-1 - (rand() % std::min(3, n-1)); // Select a random index among the three less frequent beverages

        Beverage* suggestion = m_beverages[randomIdx];

        // Remove the suggestion from its current position
        m_displayBeverages.erase(
            std::remove(m_displayBeverages.begin(), m_displayBeverages.end(), suggestion),
            m_displayBeverages.end()
        );

        // Insert the suggestion at index 2
        m_displayBeverages.insert(m_displayBeverages.begin() + 2, suggestion);
    }

    qInfo() << "   Display beverages updated:";
    for (int i = 0; i < m_displayBeverages.size(); i++) {
        qInfo() << "      Display beverage [" << i << "]: " << m_displayBeverages[i]->name();
    }

    emit displayBeveragesChanged();
}