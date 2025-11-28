#ifndef BEVERAGE_H
#define BEVERAGE_H


#include <QList>            // Include QList for list of beverages
#include <QObject>          // Include QObject for Q_OBJECT macro
#include <QQmlListProperty> // Include QQmlListProperty for QML list properties
#include <QString>          // Include QString for name property
#include <QVariantMap>      // Include QVariantMap for ingredients property

/**
 * @class Beverage
 * @brief Represents a beverage with configurable ingredients.
 *
 * The `Beverage` class models a drink with various ingredients 
 * that are stored in a map. Each ingredient is represented by a name and
 * a IngredientInfo struct that holds information about the ingredient's.
 * The class also provides the static list of the default beverages.
 *
 * ### Attributes:
 * - **name**: The name of the beverage (read-only).
 * - **ingredients**: A map of ingredient names to their corresponding IngredientInfo.
 * - **defaultBeverageList**: A static list of default beverage instances.

 */
class Beverage : public QObject { // Inherit from QObject
  Q_OBJECT                          // Enable Qt's meta-object features
  // Define properties for QML access
  Q_PROPERTY(
      QString name READ name CONSTANT) // Name of the beverage
  Q_PROPERTY(
      QVariantMap ingredients READ ingredients NOTIFY ingredientsChanged) // Map of ingredient names to IngrendientInfo

public:
  explicit Beverage(const QString &name, float coffeeBeans, float minCoffeeBeans, float maxCoffeeBeans,
                        float cocoaPowder, float minCocoaPowder, float maxCocoaPowder,
                        float water, float minWater, float maxWater,
                        float foam, float minFoam, float maxFoam,
                        float milk, float minMilk, float maxMilk,
                        QObject *parent = nullptr); // Constructor

  QString name() const;                             // Getter for name property
  QVariantMap ingredients(); // Getter for ingredients property


  Q_INVOKABLE void
  setIngredientCurrent(const QString &name, float value); // Method to set current value of an ingredient

  Q_INVOKABLE void
  resetIngredients(); // Method to reset ingredients to default values


  static void
  initDefaultBeveragesList(); // Initializes the static default beverage list
  static QList<Beverage *>
  getIndependentBeverageList(); // Returns a deep copy of the default beverage
                                // list, with new Beverage instances.
  Q_INVOKABLE int
  brewingTime(); // Method to calculate brewing time based on ingredients

private:
  // Data members
  QString m_name;
  QVariantMap m_ingredients; // Private Map of ingredient names to IngredientInfo
  static QList<Beverage *>
      s_defaultBeverageList; // Static list of default beverages

signals:
    void ingredientsChanged(); // Signal emitted when ingredients change
};

#endif // BEVERAGE_H