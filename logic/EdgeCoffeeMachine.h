#ifndef EDGE_COFFEE_MACHINE_H
#define EDGE_COFFEE_MACHINE_H

#include <QObject> // Include QObject for Q_OBJECT macro
#include <QVariantList> // Include QVariantList if needed
#include <QQmlListProperty> // Include QQmlListProperty for QML list properties
#include <vector>
#include <string>
#include <memory>

#include "Beverage.h" // Include the Beverage class

class EdgeCoffeeMachine : public QObject { // Inherit from QObject
    Q_OBJECT // Enable Qt's meta-object features

    // Define properties for QML access
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(bool isMakingDrink READ isMakingDrink WRITE setIsMakingDrink NOTIFY isMakingDrinkChanged)
    Q_PROPERTY(QQmlListProperty<Beverage> beverages READ getBeverages NOTIFY beveragesChanged)
    Q_PROPERTY(Beverage* selectedBeverage READ selectedBeverage NOTIFY selectedBeverageChanged)

public:
    explicit EdgeCoffeeMachine(QObject *parent = nullptr); // Constructor with QObject parent

    // Q_INVOKABLE methods to be called from QML
    Q_INVOKABLE void addBeverage(const QString& name);
    Q_INVOKABLE void removeBeverage(const QString& name);
    Q_INVOKABLE void makeDrink(const QString& drinkName);
    Q_INVOKABLE void selectBeverage(const QString& name);

    // Getter methods for properties
    Beverage* selectedBeverage() const;
    QString status() const;
    bool isMakingDrink() const;
    QQmlListProperty<Beverage> getBeverages();

private:
    Beverage* getBeverage(const QString& name); // Helper to find a beverage by name

    // Data members
    std::vector<std::unique_ptr<Beverage>> beverages;
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
    void beveragesChanged();
    void selectedBeverageChanged();
};

#endif // EDGE_COFFEE_MACHINE_H
