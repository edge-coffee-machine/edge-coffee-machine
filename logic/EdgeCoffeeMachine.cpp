#include <QDebug>     // Debugging utilities
#include <QQmlEngine> // For QQmlListProperty
#include <QTimer>     // Timer for simulating drink making
#include <algorithm>  // For std::remove_if

#include "EdgeCoffeeMachine.h" // Include the EdgeCoffeeMachine

// Creation of the singleton instance
EdgeCoffeeMachine &EdgeCoffeeMachine::instance() {
  static EdgeCoffeeMachine instance;
  return instance;
}

// Constructor
EdgeCoffeeMachine::EdgeCoffeeMachine(QObject *parent)
    : QObject(parent), m_status("Idle"), m_isMakingDrink(false) {
  m_weightedBeverages = WeightedSortedList<Beverage *>(
      Beverage::getIndependentBeverageList(), popularityWeightR);

  // print weighted beverages
  qDebug() << "[ECM] Initial popular beverages and weights:";
  const QList<Beverage *> &beverages = m_weightedBeverages.items();
  const QVector<float> &weights = m_weightedBeverages.weights();
  for (int i = 0; i < beverages.size(); ++i) {
    qDebug() << "   Beverage [" << i << "]: " << beverages[i]->name()
             << ", weight =" << weights[i];
  }

  // Select a default beverage if the list is not empty
  if (m_weightedBeverages.size() != 0) {
    // Set the most popular beverage as the default selection
    m_selectedBeverage = m_weightedBeverages.items().first();
  } else {
    qWarning("EdgeCoffeeMachine initialized with an empty beverage list!");
    m_selectedBeverage = nullptr; // Remains nullptr if no drinks are available
  }
}

void EdgeCoffeeMachine::test() {
  m_user = new User("Test User", 0, this);
  m_user->test();
}

void EdgeCoffeeMachine::recordBeverageSelection(const QString name) {
  const QList<Beverage *> items = m_weightedBeverages.items();
  for (int i = 0; i < items.size(); ++i) {
    if (items[i]->name() == name) {
      m_weightedBeverages.recordSelectionAt(i);

      qInfo() << "[ECM] Recorded beverage selection: " << name;

      // Print weights
      const QList<Beverage *> &beverages = m_weightedBeverages.items();
      const QVector<float> &weights = m_weightedBeverages.weights();
      for (int j = 0; j < beverages.size(); ++j) {
        qInfo() << "   Beverage [" << j << "]: " << beverages[j]
                << ", weight =" << weights[j];
      }

      emit beveragesChanged(); // Notify QML that the beverage list has changed
      return;
    }
  }

  qWarning() << "Beverage name not found in popularity list: " << name;
}

// Q_PROPERTY getters
QString EdgeCoffeeMachine::status() const { return m_status; }

bool EdgeCoffeeMachine::isMakingDrink() const { return m_isMakingDrink; }

// Private setters with signal emission
void EdgeCoffeeMachine::setStatus(const QString &status) {
  if (m_status != status) {
    m_status = status;
    emit statusChanged(); // Emit signal when status changes
  }
}

void EdgeCoffeeMachine::setIsMakingDrink(bool making) {
  if (m_isMakingDrink != making) {
    m_isMakingDrink = making;
    emit isMakingDrinkChanged(); // Emit signal when making drink state changes
  }
}

// Q_PROPERTY getter for QQmlListProperty<Beverage>
QQmlListProperty<Beverage> EdgeCoffeeMachine::getBeverages() {
  if (m_user) {
    return m_user->displayBeverages();
  } else {
    return QQmlListProperty<Beverage>(
        this, const_cast<QList<Beverage *> *>(&m_weightedBeverages.items()));
  }
}

const QList<Beverage *> &EdgeCoffeeMachine::getPopularBeverages() {
  return m_weightedBeverages.items();
}

User *EdgeCoffeeMachine::user() const { return m_user; }

void EdgeCoffeeMachine::makeDrink(Beverage *beverage) {
  if (m_isMakingDrink) {
    setStatus("Already making a drink. Please wait.");
    return;
  }

  if (!beverage) {
    setStatus("Invalid beverage.");
    return;
  }

  QString drinkName = beverage->name();

  setIsMakingDrink(true);                        // Set making drink state
  setStatus("Making " + drinkName + "...");      // Update status
  qDebug() << "Starting to make: " << drinkName; // Debug output

  int timeToBrew = beverage->brewingTime(); // calculate brewing time
  qDebug() << "Estimated brewing time (ms): " << timeToBrew;

  // Simulate work with a timer
  QTimer::singleShot(timeToBrew, this, [this, drinkName, beverage]() {
    // In a real machine, this would involve checking ingredients,
    // dispensing, heating, etc.
    recordBeverageSelection(drinkName); // Record selection of the made drink,
                                        // for popularity tracking
    if (m_user) {
      m_user->beverageBrewed(beverage); // Notify user about the brewed beverage
      m_user = nullptr;                 // Logout user after brewing
      emit userChanged();
    }
    setStatus(drinkName + " is ready!");          // Update status
    setIsMakingDrink(false);                      // Reset making drink state
    qDebug() << "Finished making: " << drinkName; // Debug output
  });
}

// Getter for the selected beverage property
Beverage *EdgeCoffeeMachine::selectedBeverage() const {
  return m_selectedBeverage;
}

// Method to select a beverage by name
void EdgeCoffeeMachine::selectBeverage(Beverage *beverage) {
  if (m_selectedBeverage != beverage) {
    m_selectedBeverage = beverage;
    emit selectedBeverageChanged(); // Notify QML that the selection has changed
  }
}
