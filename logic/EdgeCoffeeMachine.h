#ifndef EDGE_COFFEE_MACHINE_H
#define EDGE_COFFEE_MACHINE_H

#include <QObject> // Include QObject for Q_OBJECT macro
#include <QVariantList> // Include QVariantList if needed
#include <QQmlListProperty> // Include QQmlListProperty for QML list properties
#include <QList> // Include QList for list of beverages
#include <vector>
#include <string>
#include <memory>

#include "Beverage.h" // Include the Beverage class
#include "User.h" //Include the User class
#include "WeightedSortedList.h" // Include the WeightedSortedList template

class EdgeCoffeeMachine : public QObject { // Inherit from QObject
    Q_OBJECT // Enable Qt's meta-object features

    // Define properties for QML access
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(bool isMakingDrink READ isMakingDrink WRITE setIsMakingDrink NOTIFY isMakingDrinkChanged)
    Q_PROPERTY(QQmlListProperty<Beverage> beverages READ getBeverages NOTIFY beveragesChanged)
    Q_PROPERTY(Beverage* selectedBeverage READ selectedBeverage NOTIFY selectedBeverageChanged)
    Q_PROPERTY(User* user READ user NOTIFY userChanged) // Expose current user to QML (may be nullptr if no user logged in)

public:
    // Static method to get the singleton instance
    static EdgeCoffeeMachine& instance();

    // Delete copy constructor and assignment operator to prevent copies
    EdgeCoffeeMachine(const EdgeCoffeeMachine&) = delete;
    EdgeCoffeeMachine& operator=(const EdgeCoffeeMachine&) = delete;

    void test();
    
    // Q_INVOKABLE methods to be called from QML
    Q_INVOKABLE void makeDrink(Beverage* beverage);
    Q_INVOKABLE void selectBeverage(Beverage* beverage);
    
    // Getter methods for properties
    Beverage* selectedBeverage() const;
    QString status() const;
    bool isMakingDrink() const;
    QQmlListProperty<Beverage> getBeverages(); //For QML access
    const QList<Beverage*>& getPopularBeverages(); // For User access, NOT for QML access
    User* user() const; // Getter for QML user property
    
private:
    explicit EdgeCoffeeMachine(QObject *parent = nullptr); // Constructor with QObject parent
    
    void recordBeverageSelection(const QString name);

    inline static constexpr float popularityWeightR = 0.175f; // Weight update rate for the popularity list
    
    // Data members
    User* m_user = nullptr; // Current user
    WeightedSortedList<Beverage*> m_weightedBeverages;
    QString m_status;
    bool m_isMakingDrink;
    Beverage* m_selectedBeverage = nullptr;
    
    // Setter methods for properties
    void setStatus(const QString& status);
    void setIsMakingDrink(bool making);

signals:
    // Signal emitted when properties change
    void statusChanged();
    void isMakingDrinkChanged();
    void selectedBeverageChanged();
    void beveragesChanged();
    void userChanged();
};

#endif // EDGE_COFFEE_MACHINE_H
