#include <QGuiApplication> //Manages the application lifecycle and key events
#include <QQmlApplicationEngine> // Loads QML files and manages the QML engine
#include <QQmlContext> // Provides context for QML engine to access C++ objects
#include <QQmlEngine> // Core QML engine functionalities
#include <QDebug> // Debugging utilities    

#include "EdgeCoffeeMachine.h" // Include the EdgeCoffeeMachine header
#include "Beverage.h" // Include the Beverage header

int main(int argc, char *argv[]) {
    using namespace Qt::StringLiterals;

    QGuiApplication app(argc, argv); // Create the application instance
    QQmlApplicationEngine engine; // Create the QML engine to load QML files and draw the UI

    EdgeCoffeeMachine edgeCoffeeMachine;
    qmlRegisterType<Beverage>("com.example.coffeemachine", 1, 0, "Beverage"); // Register Beverage type with QML
    engine.rootContext()->setContextProperty("edgeCoffeeMachineController", &edgeCoffeeMachine); // Expose EdgeCoffeeMachine to QML

    const QUrl url(u"qrc:/EdgeCoffeeMachine/main.qml"_s); // URL of the main QML file
    // Handle object creation failure
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.load(url); // Load the main QML file

    return app.exec(); // Start the application event loop
}