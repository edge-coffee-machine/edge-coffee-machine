#include <QDebug>          // Debugging utilities
#include <QGuiApplication> //Manages the application lifecycle and key events
#include <QQmlApplicationEngine> // Loads QML files and manages the QML engine
#include <QQmlContext> // Provides context for QML engine to access C++ objects
#include <QQmlEngine>  // Core QML engine functionalities
#include <QQuickStyle> // Allows setting the application style

#include "Beverage.h"          // Include the Beverage header
#include "EdgeCoffeeMachine.h" // Include the EdgeCoffeeMachine header

int main(int argc, char *argv[]) {
  QQuickStyle::setStyle("Basic"); // Use Basic style for customization
  using namespace Qt::StringLiterals;
  Beverage::initDefaultBeveragesList();

  QGuiApplication app(argc, argv); // Create the application instance
  QQmlApplicationEngine
      engine; // Create the QML engine to load QML files and draw the UI

  EdgeCoffeeMachine &edgeCoffeeMachine = EdgeCoffeeMachine::instance();
  edgeCoffeeMachine.test();
  qmlRegisterType<Beverage>("com.example.coffeemachine", 1, 0,
                            "Beverage"); // Register Beverage type with QML
  // edgeCoffeeMachine.test(); // Run test to initialize user and beverages
  engine.rootContext()->setContextProperty(
      "edgeCoffeeMachineController",
      &edgeCoffeeMachine); // Expose EdgeCoffeeMachine to QML

  const QUrl url(
      u"qrc:/EdgeCoffeeMachine/main.qml"_s); // URL of the main QML file
  // Handle object creation failure
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.load(url); // Load the main QML file

  return app.exec(); // Start the application event loop
}
