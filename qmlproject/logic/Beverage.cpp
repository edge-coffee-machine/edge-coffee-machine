#include "Beverage.h"
#include "IngredientInfo.h"

// Implementations for Beverage

// Constructor
Beverage::Beverage(const QString &name, float coffeeBeans, float minCoffeeBeans, float maxCoffeeBeans,
                        float cocoaPowder, float minCocoaPowder, float maxCocoaPowder,
                        float water, float minWater, float maxWater,
                        float foam, float minFoam, float maxFoam,
                        float milk, float minMilk, float maxMilk,
                        QObject *parent)
    : QObject(parent), m_name(name) {
  
  if(qBound(0.0f, coffeeBeans, 1.0f)>0){
    m_ingredients["coffeeBeans"] = QVariant::fromValue(IngredientInfo{qBound(0.0f, coffeeBeans, 1.0f), minCoffeeBeans, maxCoffeeBeans, qBound(0.0f, coffeeBeans, 1.0f)});
  }

  if(qBound(0.0f, cocoaPowder, 1.0f)>0){
    m_ingredients["cocoaPowder"] = QVariant::fromValue(IngredientInfo{qBound(0.0f, cocoaPowder, 1.0f), minCocoaPowder, maxCocoaPowder, qBound(0.0f, cocoaPowder, 1.0f)});
  }

  if(qBound(0.0f, water, 1.0f)>0){
    m_ingredients["water"] = QVariant::fromValue(IngredientInfo{qBound(0.0f, water, 1.0f), minWater, maxWater, qBound(0.0f, water, 1.0f)});
  }

  if(qBound(0.0f, foam, 1.0f)>0){
    m_ingredients["foam"] = QVariant::fromValue(IngredientInfo{qBound(0.0f, foam, 1.0f), minFoam, maxFoam, qBound(0.0f, foam, 1.0f)});
  }

  if(qBound(0.0f, milk, 1.0f)>0){
    m_ingredients["milk"] = QVariant::fromValue(IngredientInfo{qBound(0.0f, milk, 1.0f), minMilk, maxMilk, qBound(0.0f, milk, 1.0f)});
  }
}

QList<Beverage *> Beverage::s_defaultBeverageList;

QString Beverage::name() const { // Getter for name property
  return m_name;
}

QVariantMap Beverage::ingredients() { // Getter for ingredients property
  return m_ingredients;
}

void Beverage::setIngredientCurrent(const QString &name, float value) { // Method to set current value of an ingredient

  if (!m_ingredients.contains(name)) {
    qWarning() << "Ingredient not found:" << name;
    return;
  }
  

  IngredientInfo info = m_ingredients[name].value<IngredientInfo>();
  info.m_current = qBound(info.m_min, value, info.m_max);
  m_ingredients[name] = QVariant::fromValue(info);
  emit ingredientsChanged();
  
}
void Beverage::resetIngredients() { // Method to reset ingredients to default
                                    // values
  for (const QString &ingredient : m_ingredients.keys()) {
    IngredientInfo info = m_ingredients[ingredient].value<IngredientInfo>();
    info.m_current = info.m_def;
    m_ingredients[ingredient] = QVariant::fromValue(info);
  }
  emit ingredientsChanged();
}

// Static method to initialize the static list of default beverages.
// This method should be called once at application startup
void Beverage:: initDefaultBeveragesList() {
    qDeleteAll(Beverage::s_defaultBeverageList);
    Beverage::s_defaultBeverageList.clear();

    // Espresso: coffee and water
    Beverage::s_defaultBeverageList.append(
        new Beverage("Espresso", 0.8f, 5.0f, 20.0f,  // coffeeBeans
                     0.0f, 0.0f, 0.0f,              // cocoaPowder
                     0.2f, 30.0f, 60.0f,            // water
                     0.0f, 0.0f, 0.0f,              // foam
                     0.0f, 0.0f, 0.0f));            // milk

    // Cappuccino: coffee, water, milk, and foam
    Beverage::s_defaultBeverageList.append(
        new Beverage("Cappuccino", 0.6f, 5.0f, 15.0f,  // coffeeBeans
                     0.0f, 0.0f, 0.0f,                // cocoaPowder
                     0.2f, 30.0f, 50.0f,              // water
                     0.1f, 10.0f, 30.0f,              // foam
                     0.3f, 50.0f, 100.0f));           // milk

    // Latte: coffee, water, and a lot of milk
    Beverage::s_defaultBeverageList.append(
        new Beverage("Latte", 0.5f, 5.0f, 15.0f,       // coffeeBeans
                     0.0f, 0.0f, 0.0f,                // cocoaPowder
                     0.2f, 30.0f, 50.0f,              // water
                     0.0f, 0.0f, 10.0f,               // foam
                     0.6f, 100.0f, 200.0f));          // milk

    // Americano: coffee and a lot of water
    Beverage::s_defaultBeverageList.append(
        new Beverage("Americano", 0.7f, 5.0f, 15.0f,   // coffeeBeans
                     0.0f, 0.0f, 0.0f,                // cocoaPowder
                     0.8f, 100.0f, 300.0f,            // water
                     0.0f, 0.0f, 0.0f,                // foam
                     0.0f, 0.0f, 0.0f));              // milk

    // Mocha: coffee, water, milk, foam, and cocoa
    Beverage::s_defaultBeverageList.append(
        new Beverage("Mocha", 0.5f, 5.0f, 15.0f,       // coffeeBeans
                     0.3f, 5.0f, 10.0f,               // cocoaPowder
                     0.2f, 30.0f, 50.0f,              // water
                     0.1f, 10.0f, 20.0f,              // foam
                     0.4f, 50.0f, 100.0f));           // milk

    // Milk: only milk
    Beverage::s_defaultBeverageList.append(
        new Beverage("Milk", 0.0f, 0.0f, 0.0f,         // coffeeBeans
                     0.0f, 0.0f, 0.0f,                // cocoaPowder
                     0.0f, 0.0f, 0.0f,                // water
                     0.0f, 0.0f, 0.0f,                // foam
                     1.0f, 100.0f, 300.0f));          // milk

    qDebug() << "Default beverages list initialized with"
             << Beverage::s_defaultBeverageList.size() << "beverages.";
}

// Static method to create an independent list of the default beverages.
// The caller is responsible for deleting the cloned beverages.
QList<Beverage *> Beverage::getIndependentBeverageList() {
    QList<Beverage *> clonedList;

    // Iterate through the default beverage list
    for (Beverage *bev : Beverage::s_defaultBeverageList) {
        // Variables to store ingredient values for the new beverage
        float coffeeBeans = 0.0f, minCoffeeBeans = 0.0f, maxCoffeeBeans = 0.0f;
        float cocoaPowder = 0.0f, minCocoaPowder = 0.0f, maxCocoaPowder = 0.0f;
        float water = 0.0f, minWater = 0.0f, maxWater = 0.0f;
        float foam = 0.0f, minFoam = 0.0f, maxFoam = 0.0f;
        float milk = 0.0f, minMilk = 0.0f, maxMilk = 0.0f;

        //Reusable IngredientInfo variable
        IngredientInfo info;

        // Check if each ingredient exists and has a default value greater than 0
        if (bev->m_ingredients.contains("coffeeBeans")) {
            info = bev->m_ingredients["coffeeBeans"].value<IngredientInfo>();
            if (info.m_def > 0.0f) {
                coffeeBeans = info.m_def;
                minCoffeeBeans = info.m_min;
                maxCoffeeBeans = info.m_max;
            }
        }

        if (bev->m_ingredients.contains("cocoaPowder")) {
            info = bev->m_ingredients["cocoaPowder"].value<IngredientInfo>();
            if (info.m_def > 0.0f) {
                cocoaPowder = info.m_def;
                minCocoaPowder = info.m_min;
                maxCocoaPowder = info.m_max;
            }
        }

        if (bev->m_ingredients.contains("water")) {
            info = bev->m_ingredients["water"].value<IngredientInfo>();
            if (info.m_def > 0.0f) {
                water = info.m_def;
                minWater = info.m_min;
                maxWater = info.m_max;
            }
        }

        if (bev->m_ingredients.contains("foam")) {
            info = bev->m_ingredients["foam"].value<IngredientInfo>();
            if (info.m_def > 0.0f) {
                foam = info.m_def;
                minFoam = info.m_min;
                maxFoam = info.m_max;
            }
        }

        if (bev->m_ingredients.contains("milk")) {
            info = bev->m_ingredients["milk"].value<IngredientInfo>();
            if (info.m_def > 0.0f) {
                milk = info.m_def;
                minMilk = info.m_min;
                maxMilk = info.m_max;
            }
        }

        // Create a new beverage using the valid ingredients
        Beverage *newBeverage = new Beverage(
            bev->name(),
            coffeeBeans, minCoffeeBeans, maxCoffeeBeans,
            cocoaPowder, minCocoaPowder, maxCocoaPowder,
            water, minWater, maxWater,
            foam, minFoam, maxFoam,
            milk, minMilk, maxMilk);

        // Add the cloned beverage to the list
        clonedList.append(newBeverage);
    }

    return clonedList;
}
// Method to calculate brewing time based on ingredients. The implementation can
// be adjusted as needed.
int Beverage::brewingTime() {
  float time = 0.0;
  for (const QString &ingredient : m_ingredients.keys()) {
    IngredientInfo info = m_ingredients[ingredient].value<IngredientInfo>();
    time +=0.5 + info.m_current * 10.0f; // Example: each ingredient adds time based on its current value
  }
  return time * 1000; // Return time in milliseconds
}
