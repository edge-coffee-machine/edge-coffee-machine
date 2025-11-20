#include "User.h"
#include <QDebug>
#include "EdgeCoffeeMachine.h"


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
    
    beverageBrewed(m_weightedBeverages.items()[2]);
    qInfo() << "Hello";

    //print result of displayBeverages
    QQmlListProperty<Beverage> displayProp = displayBeverages();
    QList<Beverage*>* displayList = static_cast<QList<Beverage*>*>(displayProp.data);
    qInfo() << "   Display beverages:";
    for (int i = 0; i < displayList->size(); i++) {
        Beverage* b = (*displayList)[i];
        qInfo() << "      Beverage [" << i << "]: " << b->name()
                << ", weight =" << m_weightedBeverages.weightFor(b);
    }

    //Select beverages
    beverageBrewed(m_weightedBeverages.items()[0]);

    displayProp = displayBeverages();
    displayList = static_cast<QList<Beverage*>*>(displayProp.data);
    qInfo() << "   Display beverages:";
    for (int i = 0; i < displayList->size(); i++) {
        Beverage* b = (*displayList)[i];
        qInfo() << "      Beverage [" << i << "]: " << b->name()
                << ", weight =" << m_weightedBeverages.weightFor(b);
    }

    beverageBrewed(m_weightedBeverages.items()[4]);

    displayProp = displayBeverages();
    displayList = static_cast<QList<Beverage*>*>(displayProp.data);
    qInfo() << "   Display beverages:";
    for (int i = 0; i < displayList->size(); i++) {
        Beverage* b = (*displayList)[i];
        qInfo() << "      Beverage [" << i << "]: " << b->name()
                << ", weight =" << m_weightedBeverages.weightFor(b);
    }

    beverageBrewed(m_weightedBeverages.items()[2]);
    beverageBrewed(m_weightedBeverages.items()[3]);
    beverageBrewed(m_weightedBeverages.items()[1]);
}

QQmlListProperty<Beverage> User::displayBeverages()
{
    qInfo() << "=== User::displayBeverages called. ===";
    if (m_category != UserCategory::Default) {
        return QQmlListProperty<Beverage>(this, &m_displayBeverages);
    } 
    else {
        // Obtener la lista de popularidad global (QQmlListProperty::data es un void* al QList)
        QQmlListProperty<Beverage> popularProp = EdgeCoffeeMachine::instance().getPopularBeverages();
        QList<Beverage*>* popularList = static_cast<QList<Beverage*>*>(popularProp.data);

        if (!popularList) {
            // Fallback: usar la lista local ordenada por peso
            m_displayBeverages = m_weightedBeverages.items();
            return QQmlListProperty<Beverage>(this, &m_displayBeverages);
        }

        // Mapear las bebidas del usuario por nombre para búsqueda rápida
        QList<Beverage*> userItems = m_weightedBeverages.items();
        QHash<QString, Beverage*> nameMap;
        for (Beverage* b : userItems) {
            if (b) nameMap.insert(b->name(), b);
        }

        // Construir la lista ordenada según la lista de popularidad global
        QList<Beverage*> ordered;
        ordered.reserve(popularList->size());
        for (Beverage* p : *popularList) {
            if (!p) continue;
            Beverage* match = nameMap.value(p->name(), nullptr);
            if (match) ordered.append(match);
        }

        // Añadir cualquier bebida del usuario que no apareciese en la lista de popularidad
        for (Beverage* b : userItems) {
            if (b && !ordered.contains(b)) ordered.append(b);
        }

        m_displayBeverages = ordered;
        return QQmlListProperty<Beverage>(this, &m_displayBeverages);
    }
}

void User::beverageBrewed(Beverage* beverage)
{
    if (!beverage) return;

    qInfo() << "=== User::beverageBrewed called for beverage:" << beverage->name() << "===";

    // Locate the beverage index
    int idx = m_weightedBeverages.indexOf(beverage);

    if (idx < 0) {
        qWarning() << "   User::beverageBrewed: Beverage not found!";
        return;
    }

    classifyUser(idx);
    m_weightedBeverages.recordSelectionAt(idx);

    //print the beverage weights for debugging
    qInfo() << "   Beverage weights after selection:";
    for (int i = 0; i < m_weightedBeverages.size(); i++) {
        qInfo() << "      Beverage [" << i << "]: " << m_weightedBeverages.items()[i]->name()
                << ", weight =" << m_weightedBeverages.weightAt(i);
    }

    updateDisplayBeverages();
    EdgeCoffeeMachine::instance().recordBeverageSelection(beverage->name());

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
