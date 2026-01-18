#include "main.h"             // Your generated QML class header
#include <qul/application.h>  // Required for Qul::Application
#include <qul/qul.h>          // Required for Qul::initHardware/initPlatform

extern "C" {
    void app_start();
}

void app_start()
{
    // 1. Initialize Hardware and Platform (Standard Qul 2.x flow)
    Qul::initHardware();
    Qul::initPlatform();

    // 2. Create the Application instance
    static Qul::Application app;

    // 3. Instantiate your root QML component
    // We use 'struct main' to avoid conflict if the compiler assumes 'main' is the function
    static struct main item; 
    
    // 4. Set the item as the root of the application
    app.setRootItem(&item);

    // 5. Enter the event loop
    app.exec();
}