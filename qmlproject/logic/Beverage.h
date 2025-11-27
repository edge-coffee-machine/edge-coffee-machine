
#ifndef BEVERAGE_H
#define BEVERAGE_H

#define MAX_COFFEE_BEANS 100.0f
#define MAX_COCOA_POWDER 100.0f
#define MAX_WATER 1000.0f
#define MAX_FOAM 200.0f
#define MAX_MILK 500.0f

#include <qul/object.h>
#include <QString>

// Qt for MCUs compatible Beverage class
class Beverage : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(float coffeeBeans READ coffeeBeans WRITE setCoffeeBeans NOTIFY coffeeBeansChanged)
  Q_PROPERTY(float cocoaPowder READ cocoaPowder WRITE setCocoaPowder NOTIFY cocoaPowderChanged)
  Q_PROPERTY(float water READ water WRITE setWater NOTIFY waterChanged)
  Q_PROPERTY(float foam READ foam WRITE setFoam NOTIFY foamChanged)
  Q_PROPERTY(float milk READ milk WRITE setMilk NOTIFY milkChanged)
  Q_PROPERTY(float maxCoffeeBeans READ maxCoffeeBeans CONSTANT)
  Q_PROPERTY(float maxCocoaPowder READ maxCocoaPowder CONSTANT)
  Q_PROPERTY(float maxWater READ maxWater CONSTANT)
  Q_PROPERTY(float maxFoam READ maxFoam CONSTANT)
  Q_PROPERTY(float maxMilk READ maxMilk CONSTANT)

public:
  explicit Beverage(const QString &name = QString(), float coffeeBeans = 0, float cocoaPowder = 0, float water = 0, float foam = 0, float milk = 0, QObject *parent = nullptr)
    : QObject(parent),
      m_name(name),
      m_coffeeBeans(coffeeBeans),
      m_cocoaPowder(cocoaPowder),
      m_water(water),
      m_foam(foam),
      m_milk(milk),
      m_defaultCoffeeBeans(coffeeBeans),
      m_defaultCocoaPowder(cocoaPowder),
      m_defaultWater(water),
      m_defaultFoam(foam),
      m_defaultMilk(milk)
  {}

  QString name() const { return m_name; }
  float coffeeBeans() const { return m_coffeeBeans; }
  void setCoffeeBeans(float v) { if (m_coffeeBeans != v) { m_coffeeBeans = v; emit coffeeBeansChanged(); } }
  float cocoaPowder() const { return m_cocoaPowder; }
  void setCocoaPowder(float v) { if (m_cocoaPowder != v) { m_cocoaPowder = v; emit cocoaPowderChanged(); } }
  float water() const { return m_water; }
  void setWater(float v) { if (m_water != v) { m_water = v; emit waterChanged(); } }
  float foam() const { return m_foam; }
  void setFoam(float v) { if (m_foam != v) { m_foam = v; emit foamChanged(); } }
  float milk() const { return m_milk; }
  void setMilk(float v) { if (m_milk != v) { m_milk = v; emit milkChanged(); } }

  float maxCoffeeBeans() const { return MAX_COFFEE_BEANS; }
  float maxCocoaPowder() const { return MAX_COCOA_POWDER; }
  float maxWater() const { return MAX_WATER; }
  float maxFoam() const { return MAX_FOAM; }
  float maxMilk() const { return MAX_MILK; }

  float getDefaultCoffeeBeans() const { return m_defaultCoffeeBeans; }
  float getDefaultCocoaPowder() const { return m_defaultCocoaPowder; }
  float getDefaultWater() const { return m_defaultWater; }
  float getDefaultFoam() const { return m_defaultFoam; }
  float getDefaultMilk() const { return m_defaultMilk; }

  Q_INVOKABLE void resetIngredients() {
    setCoffeeBeans(m_defaultCoffeeBeans);
    setCocoaPowder(m_defaultCocoaPowder);
    setWater(m_defaultWater);
    setFoam(m_defaultFoam);
    setMilk(m_defaultMilk);
  }

  Q_INVOKABLE int brewingTime() {
    // Example calculation
    return static_cast<int>(coffeeBeans() * 10 + water() * 5);
  }

signals:
  void coffeeBeansChanged();
  void cocoaPowderChanged();
  void waterChanged();
  void foamChanged();
  void milkChanged();

private:
  QString m_name;
  float m_coffeeBeans;
  float m_cocoaPowder;
  float m_water;
  float m_foam;
  float m_milk;
  float m_defaultCoffeeBeans;
  float m_defaultCocoaPowder;
  float m_defaultWater;
  float m_defaultFoam;
  float m_defaultMilk;
};

#endif // BEVERAGE_H