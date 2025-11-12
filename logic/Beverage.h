#ifndef BEVERAGE_H
#define BEVERAGE_H

#include <QObject> // Include QObject for Q_OBJECT macro
#include <QString> // Include QString for name property
#include <QVariantMap> // Include QVariantMap for ingredients property

class Beverage : public QObject { // Inherit from QObject
    Q_OBJECT // Enable Qt's meta-object features

    // Define properties for QML access
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QVariantMap ingredients READ ingredients CONSTANT) // Map of ingredient names to amounts

public:
    explicit Beverage(const QString& name, QObject *parent = nullptr)
        : QObject(parent), m_name(name) {} // Constructor

    QString name() const { // Getter for name property
        return m_name;
    }

    QVariantMap ingredients() const { // Getter for ingredients property
        return m_ingredients;
    }

    void addIngredient(const QString& name, int amount) { // Method to add an ingredient
        m_ingredients[name] = amount;
    }

private:
    // Data members
    QString m_name;
    QVariantMap m_ingredients;
};

#endif // BEVERAGE_H