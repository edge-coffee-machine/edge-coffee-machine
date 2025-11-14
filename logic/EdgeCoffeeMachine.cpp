#include <QDebug> // Debugging utilities
#include <QTimer> // Timer for simulating drink making
#include <algorithm> // For std::remove_if
#include <QQmlEngine> // For QQmlListProperty

#include "EdgeCoffeeMachine.h" // Include the EdgeCoffeeMachine

// Constructor
EdgeCoffeeMachine::EdgeCoffeeMachine(QObject *parent)
: QObject(parent), m_status("Idle"), m_isMakingDrink(false)
{
    // Initialize with some default beverages
    addBeverage("Espresso");
    addBeverage("Cappuccino");
    addBeverage("Latte");
    addBeverage("Americano");
    addBeverage("Mocha");
    addBeverage("Tea");
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

// Q_INVOKABLE methods
void EdgeCoffeeMachine::addBeverage(const QString& name) {
    auto newBeverage = std::make_unique<Beverage>(name);
    // Add some default ingredients based on the beverage name
    if (name == "Espresso") {
        newBeverage->addIngredient("Coffee Beans", 10);
        newBeverage->addIngredient("Water", 30);
    } else if (name == "Cappuccino") {
        newBeverage->addIngredient("Coffee Beans", 10);
        newBeverage->addIngredient("Water", 30);
        newBeverage->addIngredient("Milk", 100);
    } else if (name == "Latte") {
        newBeverage->addIngredient("Coffee Beans", 10);
        newBeverage->addIngredient("Water", 30);
        newBeverage->addIngredient("Milk", 150);
    } else if (name == "Americano") {
        newBeverage->addIngredient("Coffee Beans", 10);
        newBeverage->addIngredient("Water", 100);
    } else if (name == "Mocha") {
        newBeverage->addIngredient("Coffee Beans", 10);
        newBeverage->addIngredient("Water", 30);
        newBeverage->addIngredient("Milk", 120);
        newBeverage->addIngredient("Chocolate Syrup", 20);
    } else if (name == "Tea") {
        newBeverage->addIngredient("Tea Leav es", 5);
        newBeverage->addIngredient("Water", 200);
    }

    beverages.push_back(std::move(newBeverage)); // Add to the list
    qDebug() << "Added beverage: " << name; // Debug output
    emit beveragesChanged(); // Notify QML about the change
}

void EdgeCoffeeMachine::removeBeverage(const QString& name) {
    auto it = std::remove_if(beverages.begin(), beverages.end(),
                             [&](const std::unique_ptr<Beverage>& b) {
                                 return b->name() == name;
                             }); // Find beverage by name
    if (it != beverages.end()) {
        beverages.erase(it, beverages.end()); // Remove from the list
        qDebug() << "Removed beverage: " << name; // Debug output
    } else {
        qDebug() << "Beverage not found: " << name; // Debug output if not found
    }
    emit beveragesChanged(); // Notify QML about the change
}

// Internal helper to get Beverage*
Beverage* EdgeCoffeeMachine::getBeverage(const QString& name) {
    // Find beverage by name
    for (const auto& b : beverages) {
        if (b->name().toStdString() == name) {
            return b.get(); // Return raw pointer
        }
    }
    return nullptr; // Not found
}

// Q_PROPERTY getter for QQmlListProperty<Beverage>
QQmlListProperty<Beverage> EdgeCoffeeMachine::getBeverages()
{
    return QQmlListProperty<Beverage>(this, this,
        // Modern way using lambdas
        [](QQmlListProperty<Beverage>* list) -> qsizetype {
            return reinterpret_cast<EdgeCoffeeMachine*>(list->data)->beverages.size();
        }, // Count of beverages
        [](QQmlListProperty<Beverage>* list, qsizetype index) -> Beverage* {
            return reinterpret_cast<EdgeCoffeeMachine*>(list->data)->beverages.at(index).get();
        }); // Access beverage at index
}

void EdgeCoffeeMachine::makeDrink(const QString& drinkName) {
    if (m_isMakingDrink) {
        setStatus("Already making a drink. Please wait.");
        return;
    }

    Beverage* beverageToMake = getBeverage(drinkName);
    if (!beverageToMake) { // Not found
        setStatus("Error: " + drinkName + " not found.");
        return;
    }

    setIsMakingDrink(true); // Set making drink state
    setStatus("Making " + drinkName + "..."); // Update status
    qDebug() << "Starting to make: " << drinkName; // Debug output

    // Simulate work with a timer
    QTimer::singleShot(3000, this, [this, drinkName]() {
        // In a real machine, this would involve checking ingredients,
        // dispensing, heating, etc.
        qDebug() << "Finished making: " << drinkName; // Debug output
        setStatus(drinkName + " is ready!"); // Update status
        setIsMakingDrink(false); // Reset making drink state
    });
}

// Getter for the selected beverage property
Beverage* EdgeCoffeeMachine::selectedBeverage() const
{
    return m_selectedBeverage;
}

// Method to select a beverage by name
void EdgeCoffeeMachine::selectBeverage(const QString& name) {
    Beverage* beverageToSelect = getBeverage(name);
    if (m_selectedBeverage != beverageToSelect) {
        m_selectedBeverage = beverageToSelect;
        emit selectedBeverageChanged(); // Notify QML that the selection has changed
    }
}
