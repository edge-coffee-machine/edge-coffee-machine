#include "Beverage.h"

// Implementations for Beverage

// Constructor
Beverage::Beverage(const QString& name, float coffeeBeans, float cocoaPowder, float water, float foam, float milk, QObject *parent)
    : QObject(parent), 
      m_name(name),
      m_defaultCoffeeBeans(qBound(0.0f, coffeeBeans, 1.0f)),
      m_defaultCocoaPowder(qBound(0.0f, cocoaPowder, 1.0f)),
      m_defaultWater(qBound(0.0f, water, 1.0f)),
      m_defaultFoam(qBound(0.0f, foam, 1.0f)),
      m_defaultMilk(qBound(0.0f, milk, 1.0f)) {
    // Initialize current ingredient amounts to defaults
    m_coffeeBeans = m_defaultCoffeeBeans;
    m_cocoaPowder = m_defaultCocoaPowder;
    m_water = m_defaultWater;
    m_foam = m_defaultFoam;
    m_milk = m_defaultMilk;
      }

QString Beverage::name() const { // Getter for name property
        return m_name;
}
float Beverage::coffeeBeans() const { 
    return m_coffeeBeans;
}
void Beverage::setCoffeeBeans(float coffeeBeans) {
    if (m_minCoffeeBeans != 0 || m_defaultCoffeeBeans != 0) { // Check if the coffee beans are present
        coffeeBeans = qBound(0.0f, coffeeBeans, 1.0f);
        if (m_coffeeBeans != coffeeBeans) {
            m_coffeeBeans = coffeeBeans;
            emit coffeeBeansChanged(); // Emit signal when coffeeBeans changes
        }
    }
}

float Beverage::cocoaPowder() const { 
    return m_cocoaPowder;
}
void Beverage::setCocoaPowder(float cocoaPowder) { 
    if (m_minCocoaPowder != 0 || m_defaultCocoaPowder != 0) { // Check if the cocoa powder is present
        cocoaPowder = qBound(0.0f, cocoaPowder, 1.0f);
        if (m_cocoaPowder != cocoaPowder) {
            m_cocoaPowder = cocoaPowder;
            emit cocoaPowderChanged(); // Emit signal when cocoaPowder changes
        }
    }
}
float Beverage::water() const { 
    return m_water;
}
void Beverage::setWater(float water) {
    if (m_minWater != 0 || m_defaultWater != 0) { // Check if the water is present
        water = qBound(0.0f, water, 1.0f);
        if (m_water != water) {
            m_water = water;
            emit waterChanged(); // Emit signal when water changes
        }
    }
}
float Beverage::foam() const { 
    return m_foam;
}
void Beverage::setFoam(float foam) { 
    if (m_minFoam != 0 || m_defaultFoam != 0) { // Check if the foam is present
        foam = qBound(0.0f, foam, 1.0f);
        if (m_foam != foam) {
            m_foam = foam;
            emit foamChanged(); // Emit signal when foam changes
        }
    }
}
float Beverage::milk() const { 
    return m_milk;
}
void Beverage::setMilk(float milk) { // Setter for milk property
    if (m_minMilk != 0 || m_defaultMilk != 0) { // Check if the milk is present
        milk = qBound(0.0f, milk, 1.0f);
        if (m_milk != milk) {
            m_milk = milk;
            emit milkChanged(); // Emit signal when milk changes
        }
    }
}
float Beverage::maxCoffeeBeans() const { 
    return m_maxCoffeeBeans;
}
float Beverage::minCoffeeBeans() const { 
    return m_minCoffeeBeans;
}
float Beverage::maxCocoaPowder() const { 
    return m_maxCocoaPowder;
}
float Beverage::minCocoaPowder() const { 
    return m_minCocoaPowder;
}
float Beverage::maxWater() const { 
    return m_maxWater;
}
float Beverage::minWater() const { 
    return m_minWater;
}
float Beverage::maxFoam() const { 
    return m_maxFoam;
}
float Beverage::minFoam() const { 
    return m_minFoam;
}
float Beverage::maxMilk() const { 
    return m_maxMilk;
}
float Beverage::minMilk() const { 
    return m_minMilk;
}
float Beverage::getDefaultCoffeeBeans() const {
    return m_defaultCoffeeBeans;
}
float Beverage::getDefaultCocoaPowder() const {
    return m_defaultCocoaPowder;
}
float Beverage::getDefaultWater() const {
    return m_defaultWater;
}
float Beverage::getDefaultFoam() const {
    return m_defaultFoam;
}
float Beverage::getDefaultMilk() const {
    return m_defaultMilk;
}

QQmlListProperty<Beverage> Beverage::defaultBeverageList(){
    return QQmlListProperty<Beverage>(nullptr, &s_defaultBeverageList,
        [](QQmlListProperty<Beverage>* list) -> qsizetype {
            return reinterpret_cast<QList<Beverage*>*>(list->data)->size();
        },
        [](QQmlListProperty<Beverage>* list, qsizetype index) -> Beverage* {
            return reinterpret_cast<QList<Beverage*>*>(list->data)->at(index);
        });
}



void Beverage::resetIngredients() { // Method to reset ingredients to default values
    m_coffeeBeans=m_defaultCoffeeBeans;
    m_cocoaPowder=m_defaultCocoaPowder;
    m_water=m_defaultWater;
    m_foam=m_defaultFoam;
    m_milk=m_defaultMilk;
    emit coffeeBeansChanged();
    emit cocoaPowderChanged();
    emit waterChanged();
    emit foamChanged();
    emit milkChanged();
}

// Static method to initialize the static list of default beverages.
// This method should be called once at application startup
void Beverage::initDefaultBeveragesList(){
    qDeleteAll(Beverage::s_defaultBeverageList);
    Beverage::s_defaultBeverageList.clear();

    Beverage::s_defaultBeverageList.append(new Beverage("Espresso", 0.2, 0.4, 0.3, 0, 0));
    Beverage::s_defaultBeverageList.append(new Beverage("Cappuccino", 0.1, 0, 0.3, 0, 0.1));
    Beverage::s_defaultBeverageList.append(new Beverage("Latte", 0.1, 0, 0.3, 0, 0.15));
    Beverage::s_defaultBeverageList.append(new Beverage("Americano", 0.1, 0, 1.0, 0, 0));
    Beverage::s_defaultBeverageList.append(new Beverage("Mocha", 0.1, 0.05, 0.3, 0, 0.12));
    Beverage::s_defaultBeverageList.append(new Beverage("Tea", 0, 0, 0.2, 0, 0));
    //Maybe we should notify here that the list has changed? but static methods can't emit signals.
    qDebug() << "Default beverages list initialized with" << Beverage::s_defaultBeverageList.size() << "beverages.";

}

// Static method to create an independent list of the default beverages.
// The caller is responsible for deleting the cloned beverages.
 QList<Beverage*> Beverage::getIndependentBeverageList() {
    QList<Beverage*> clonedList;
    for (Beverage* bev : Beverage::s_defaultBeverageList) {
        Beverage* newBeverage = new Beverage(
            bev->name(),
            bev->getDefaultCoffeeBeans(),
            bev->getDefaultCocoaPowder(),
            bev->getDefaultWater(),
            bev->getDefaultFoam(),
            bev->getDefaultMilk()
        );
        clonedList.append(newBeverage);
    }
    return clonedList;
}



