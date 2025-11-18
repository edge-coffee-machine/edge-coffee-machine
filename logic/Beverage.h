#ifndef BEVERAGE_H
#define BEVERAGE_H


#define MAX_COFFEE_BEANS 100.0f
#define MIN_COFFEE_BEANS 0.0f
#define MAX_COCOA_POWDER 100.0f
#define MIN_COCOA_POWDER 0.0f
#define MAX_WATER 1000.0f
#define MIN_WATER 0.0f
#define MAX_FOAM 200.0f
#define MIN_FOAM 0.0f
#define MAX_MILK 500.0f
#define MIN_MILK 0.0f



#include <QObject> // Include QObject for Q_OBJECT macro
#include <QString> // Include QString for name property
#include <QVariantMap> // Include QVariantMap for ingredients property
#include <QList> // Include QList for list of beverages
#include <QQmlListProperty> // Include QQmlListProperty for QML list properties


/**
 * @class Beverage
 * @brief Represents a beverage with configurable ingredients.
 *
 * The `Beverage` class models a drink with various ingredients such as coffee beans, cocoa powder,
 * water, foam, and milk. Each ingredient is represented as a normalized value between 0 and 1,
 * where:
 * - **0** corresponds to the minimum amount of the ingredient.
 * - **1** corresponds to the maximum amount of the ingredient.
 *
 * The normalization is based on absolute minimum and maximum values, which are defined for each
 * ingredient. These minimun and maximum values are not normalized and represent the actual limits for the
 * ingredient in the beverage.
 *
 * Each beverage has default normalized values for each ingredient, which define the beverage. The ingredients
 * amounts can be modified but the changes are represented in the non-default ingredient attributes.
 * 
 * The class also provides the static list of the default beverages.
 * 
 * ### Attributes:
 * - **name**: The name of the beverage (read-only).
 * - **coffeeBeans**: The normalized amount of coffee beans (0 to 1).
 * - **cocoaPowder**: The normalized amount of cocoa powder (0 to 1).
 * - **water**: The normalized amount of water (0 to 1).
 * - **foam**: The normalized amount of foam (0 to 1).
 * - **milk**: The normalized amount of milk (0 to 1).
 * - **max/min values**: Absolute maximum and minimum limits for each ingredient.
 * - **defaultBeverageList**: A static list of default beverage instances.
 * Each ingredient has different limits and constants and the same for all beverages(not normalized):
 * - **defaultCoffeeBeans**: Default normalized amount of coffee beans (0 to 1).
 * - **defaultCocoaPowder**: Default normalized amount of cocoa powder (0 to 1).
 * - **defaultWater**: Default normalized amount of water (0 to 1).
 * - **defaultFoam**: Default normalized amount of foam (0 to 1).
 * - **defaultMilk**: Default normalized amount of milk (0 to 1).
 */
class Beverage : public QObject { // Inherit from QObject
    Q_OBJECT // Enable Qt's meta-object features

    // Define properties for QML access
    Q_PROPERTY(QString name READ name CONSTANT) // Map of ingredient names to amounts
    Q_PROPERTY(float coffeeBeans READ coffeeBeans WRITE setCoffeeBeans NOTIFY coffeeBeansChanged) // Amount of coffee beans in the coffee
    Q_PROPERTY(float cocoaPowder READ cocoaPowder WRITE setCocoaPowder NOTIFY cocoaPowderChanged)// Amount of cocoaPowder in the coffee
    Q_PROPERTY(float water READ water WRITE setWater NOTIFY waterChanged) // Amount of water in the coffee
    Q_PROPERTY(float foam READ foam WRITE setFoam NOTIFY foamChanged) // Amount of foam in the coffee
    Q_PROPERTY(float milk READ milk WRITE setMilk NOTIFY milkChanged) // Amount of milk in the coffee
    
    Q_PROPERTY(float maxCoffeeBeans READ maxCoffeeBeans CONSTANT) // Maximum amount of coffee beans in the coffee
    Q_PROPERTY(float minCoffeeBeans READ minCoffeeBeans CONSTANT) // Minimum amount of coffee beans in the coffee
    
    Q_PROPERTY(float maxCocoaPowder READ maxCocoaPowder CONSTANT) // Maximum amount of cocoaPowder in the coffee
    Q_PROPERTY(float minCocoaPowder READ minCocoaPowder CONSTANT) // Minimum amount of cocoaPowder in the coffee
    
    Q_PROPERTY(float maxWater READ maxWater CONSTANT) // Maximum amount of water in the coffee
    Q_PROPERTY(float minWater READ minWater CONSTANT) // Minimum amount of water in the coffee

    Q_PROPERTY(float maxFoam READ maxFoam CONSTANT) // Maximum amount of foam in the coffee
    Q_PROPERTY(float minFoam READ minFoam CONSTANT) // Minimum amount of foam in the coffee

    Q_PROPERTY(float maxMilk READ maxMilk CONSTANT) // Maximum amount of milk in the coffee
    Q_PROPERTY(float minMilk READ minMilk CONSTANT) // Minimum amount of milk in the coffee

    Q_PROPERTY(QQmlListProperty<Beverage> defaultBeverageList READ defaultBeverageList NOTIFY defaultBeverageListChanged) // Static list of default beverages

public:
    explicit Beverage(const QString& name, float coffeeBeans, float cocoaPowder, float water, float foam, float milk, QObject *parent = nullptr); // Constructor
    QString name() const; // Getter for name property
    //Getters and setters for ingredient properties
    float coffeeBeans() const;
    void setCoffeeBeans(float coffeeBeans);
    float cocoaPowder() const;
    void setCocoaPowder(float cocoaPowder);
    float water() const;
    void setWater(float water);
    float foam() const;
    void setFoam(float foam);
    float milk() const;
    void setMilk(float milk);
    float maxCoffeeBeans() const;
    float minCoffeeBeans() const;
    float maxCocoaPowder() const;
    float minCocoaPowder() const;
    float maxWater() const;
    float minWater() const;
    float maxFoam() const;
    float minFoam() const;
    float maxMilk() const;
    float minMilk() const;
    float getDefaultCoffeeBeans() const;
    float getDefaultCocoaPowder() const;
    float getDefaultWater() const;
    float getDefaultFoam() const;
    float getDefaultMilk() const;
    QQmlListProperty<Beverage> defaultBeverageList();

    Q_INVOKABLE void resetIngredients();// Method to reset ingredients to default values
    static void initDefaultBeveragesList();
    static QList<Beverage*> getIndependentBeverageList(); // Returns a deep copy of the default beverage list, with new Beverage instances.

private:
    // Data members
    QString m_name;

    float m_coffeeBeans;
    float m_cocoaPowder;
    float m_water;
    float m_foam;
    float m_milk;

    const float m_defaultCoffeeBeans;
    const float m_defaultCocoaPowder;
    const float m_defaultWater;
    const float m_defaultFoam;
    const float m_defaultMilk;

    static constexpr float m_maxCoffeeBeans = MAX_COFFEE_BEANS;
    static constexpr float m_minCoffeeBeans = MIN_COFFEE_BEANS;
    
    static constexpr float m_maxCocoaPowder = MAX_COCOA_POWDER;
    static constexpr float m_minCocoaPowder = MIN_COCOA_POWDER;

    static constexpr float m_maxWater = MAX_WATER;
    static constexpr float m_minWater = MIN_WATER;

    static constexpr float m_maxFoam = MAX_FOAM;
    static constexpr float m_minFoam = MIN_FOAM;

    static constexpr float m_maxMilk = MAX_MILK;
    static constexpr float m_minMilk = MIN_MILK;

    static QList<Beverage*> s_defaultBeverageList; // Static list of default beverages
signals:

    void coffeeBeansChanged();
    void cocoaPowderChanged();
    void waterChanged();
    void foamChanged();
    void milkChanged();
    void defaultBeverageListChanged();
};

#endif // BEVERAGE_H