#include <QDebug> // Debugging utilities
#include <QTimer> // Timer for simulating drink making
#include <algorithm> // For std::remove_if
#include <QQmlEngine> // For QQmlListProperty

#include "EdgeCoffeeMachine.h" // Include the EdgeCoffeeMachine

// Creation of the singleton instance
EdgeCoffeeMachine& EdgeCoffeeMachine::instance()
{
    static EdgeCoffeeMachine EdgeCoffeeMachine_instance;
    return EdgeCoffeeMachine_instance;
}

// Constructor
EdgeCoffeeMachine::EdgeCoffeeMachine(QObject *parent)
: QObject(parent), m_status("Idle"), m_isMakingDrink(false)
{
    user = new User("Test User", 0, this);
    user->test();

    m_weightedBeverages = WeightedSortedList<Beverage*>(Beverage::getIndependentBeverageList(), popularityWeightR);
}

// QQmlListProperty append function (not implemented for now)
static void append_beverage(QQmlListProperty<Beverage>* list, Beverage* beverage) {
    Q_UNUSED(list);
    Q_UNUSED(beverage);
}

// Q_PROPERTY getters
QString EdgeCoffeeMachine::status() const {
    return m_status;
}

bool EdgeCoffeeMachine::isMakingDrink() const {
    return m_isMakingDrink;
}

// Private setters with signal emission
void EdgeCoffeeMachine::setStatus(const QString& status) {
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
QQmlListProperty<Beverage> EdgeCoffeeMachine::getBeverages()
{
    if (user) {
        return user->displayBeverages();
    }
    else {
        return getPopularBeverages();
    }
}

QQmlListProperty<Beverage> EdgeCoffeeMachine::getPopularBeverages()
{
    return QQmlListProperty<Beverage>(this, const_cast<QList<Beverage*>*>(&m_weightedBeverages.items()));
}

void EdgeCoffeeMachine::makeDrink(Beverage* beverage) {
    if (m_isMakingDrink) {
        setStatus("Already making a drink. Please wait.");
        return;
    }
    
    QString drinkName = beverage->name();

    setIsMakingDrink(true); // Set making drink state
    setStatus("Making " + drinkName + "..."); // Update status
    qDebug() << "Starting to make: " << drinkName; // Debug output

    // Simulate work with a timer
    QTimer::singleShot(3000, this, [this, drinkName, beverage]() {
        // In a real machine, this would involve checking ingredients,
        // dispensing, heating, etc.
        qDebug() << "Finished making: " << drinkName; // Debug output
        setStatus(drinkName + " is ready!"); // Update status
        setIsMakingDrink(false); // Reset making drink state
        m_weightedBeverages.recordSelection(beverage); // Record selection of the made drink, for popularity tracking
        if (user) user->beverageBrewed(beverage); // Notify user about the brewed beverage
    });
}

// Getter for the selected beverage property
Beverage* EdgeCoffeeMachine::selectedBeverage() const
{
    return m_selectedBeverage;
}

// Method to select a beverage by name
void EdgeCoffeeMachine::selectBeverage(Beverage* beverage) {
    if (m_selectedBeverage != beverage) {
        m_selectedBeverage = beverage;
        emit selectedBeverageChanged(); // Notify QML that the selection has changed
    }
}
