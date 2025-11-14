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


