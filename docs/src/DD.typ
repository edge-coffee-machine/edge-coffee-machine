#set document(
  title: [Design Document],
  keywords: "design document",
)

#set text(size: 13.3pt)
#set page(
  margin: 1.4in,
  numbering: "1",
)
#set par(
  first-line-indent: 1.4em,
  justify: true,
  linebreaks: "optimized",
  justification-limits: (
    tracking: (min: -0.025em, max: 0.0em),
  ),
)
#set enum(indent: 1em)
#set list(indent: 1em)
#set heading(numbering: "1.1.1")
#show heading.where(level: 1): it => {
  pagebreak(weak: true)
  it
}
#show figure.where(kind: table): set block(breakable: true)
#show outline.entry.where(level: 1): it => {
  v(13pt, weak: true)
  strong(it)
}
#show outline.entry.where(level: 3): it => {
  text(style: "italic", it)
}
#show heading.where(level: 5): it => text(
  weight: "bold",
  it.body,
)
#show title: set text(size: 36pt, weight: 500)
#show link: it => text(
  fill: rgb(blue),
  it,
)
#show "-th": "\u{2011}th" // Non-breaking hyphen
#show "C++": name => box(name) // Ensure the + sign isn't split across lines

// Render tables à la booktabs
// #show figure: set block(breakable: true)
#set table(
  inset: 6pt,
  stroke: (x, y) => (
    left: none,
    right: none,
    top: if y == 0 {
      1pt
    } else if y == 1 {
      0.5pt
    } else {
      0pt
    },
    bottom: 1pt,
  ),
)

// Title and author
#align(
  center,
  {
    title()
    text(size: 24pt, fill: rgb("#777"))[Edge Coffee Machine]
    linebreak()
    v(4pt)
    text(size: 1.2em)[Living document · Snapshot: 2025-12-26]
    linebreak()
    text(
      fill: rgb("#777"),
    )[_For the most up-to-date version, click #link("https://typst.app/project/RwKblhQMrTyNgKGXXCDx0F")[here]._]
    v(16pt)
    grid(
      columns: 2,
      row-gutter: 4pt,
      column-gutter: 16pt,
      align: left,
      [Piervito Creanza], [Javier Asensio Castillo],
      [Matteo Delton], [Jorge Jiménez Oropesa],
      [Emiliano Finetti], [Balša Rakočević],
      [Maxime Trimboli],
    )
    align(
      grid(
        columns: 2,
        gutter: 15%,
        image("logos/polimi_logo.svg"), image("logos/mdu_logo.svg"),
      ),
      bottom,
    )
  },
)

#pagebreak(weak: true)
#outline(indent: 1em, depth: 3)
#pagebreak(weak: true)

= Introduction & Background

== Document Purpose

The purpose of this Design Document is to detail the architectural and technical specifications for the Edge Coffee Machine. Building on the Requirements Analysis and Specification Document (RASD), this document serves as a comprehensive guide for the system's implementation. It aims to assist developers, architects, and other stakeholders in understanding the structural design, component interactions, and technical decisions that will define the Edge Coffee Machine.

== Background

=== Team & Collaboration

The ECM is an academic group project for the #link("https://www.fer.unizg.hr/rasip/dsd")[Distributed Software Development course], emphasizing international development collaboration. The development team includes a total of 7 students, including 4 students from the Politecnico of Milano (PoliMi, Italy) and 3 students from Mälardalen University (MDU, Sweden). This team is also closely supported by one coordinator from each university.

=== Customer

The customer for this project is the Qt Group, a software company primarily known for its open-source User Interface (UI) frameworks. The company provides products like #link("https://www.qt.io/platform/develop-software-microcontrollers-mcu")[Qt for MCU], which enables the creation of "smartphone-like user experiences" on resource-constrained embedded devices.

=== Objectives

The ECM project focuses on demonstrating the capabilities of Edge AI on microcontrollers by creating an AI-driven coffee machine application.
However, our objective is not only to showcase the technical power of Edge AI; we also aim to illustrate how technology can anticipate and respond to human needs in everyday moments.
The system will recognize each user, remember their preferences, and offer a personalized coffee experience that feels natural and effortless. By running AI directly on the device, the Edge Coffee Machine ensures instant responses, enhanced privacy, and energy-efficient performance.

== Project Scope

This design document covers the development of a fully functional application for the Edge Coffee Machine. The scope encompasses:

- User Interface: Creating a fluid, high-performance interface that handles all user interactions (touchscreen, camera image recognition, and voice commands).

- Application Logic: Implementing the core business logic: mainly customer classification and personalized drink suggestions.

- AI Component: Integrating AI models for image recognition and voice commands on the target microcontroller.

The integration with the physical coffee machine hardware and the handling of payments are out of scope.

= High-Level System Description

At a high level, the Edge Coffee Machine is designed as an autonomous embedded system that recognizes users and tailors its interface and functionalities to their preferences. The system consists of three main layers: Input Layer, Processing Layer, and Output Layer.

== Input Layer

The system collects data from multiple input sources:

- *Camera*: Captures an image of the user to perform facial recognition.

- *Microphone*: Captures predefined voice commands to control menu navigation and drink selection.

- *Touchscreen Display*: Allows manual user input for customization or confirmation of selections.

== System Interfaces

This section defines the external interfaces of the system that enable interaction with users and hardware components.

=== User Interfaces

- _Touch Interface_ (Qt for MCU): Primary means of interaction for drink customization and confirmation.

- _Voice Interface_: Allows users to issue specific commands such as “make espresso” or “show menu.”

- _Vision Interface_ (Camera): Detects and recognizes users automatically without manual input.

=== Software Interfaces

- _Frontend (Qt for MCU)_: Responsible for rendering the GUI and handling user input.

- _Backend (ESP-IDF in C++)_: Contains the core application logic, communication with the hardware peripherals, and management of AI inference results.

- _AI Runtime Libraries (ESP-DL or emlearn)_: Used to execute trained machine learning models for facial and voice recognition.

- _Internal Communication Protocols_: Communication between modules (e.g., between backend and AI runtime) is handled via in-memory message passing within the microcontroller's environment.

=== Hardware Interfaces

The ESP32-P4-Function-EV microcontroller connects directly to:

- Camera module (for vision input)

- Microphone (for audio input)

- Touch display (for interaction and visual output)

All components are integrated into a single development board platform, eliminating the need for external hardware dependencies.

== Hardware Overview

The ESP32-P4-Function-EV microcontroller is the core of the system.
It is equipped with:

- A dual-core processor optimized for parallel multimedia tasks.

- A Neural Processing Unit (NPU) to accelerate AI inference.

- A 7” capacitive touchscreen display.

- A camera for face detection and recognition.

- A microphone for capturing voice commands.

- Multiple I/O ports for possible future extensions.

The use of this hardware enables real-time AI execution at the edge, supporting the project’s primary design principle: intelligent interaction without external dependency.

#figure(
  image("diagrams/architecture/block_diagram.png"),
  caption: [
    ESP32-P4-Function-EV board block diagram.
  ],
)

= Architectural Design

== Overview

This chapter details the high-level software architecture for the ECM. It defines system design principles, including the Modular Monolith structure and the Edge AI approach.
\ The architecture is broken down into its primary logical components (_UI/Frontend_, _App logic/Backend_ and _AI component_) which are needed to fulfill the system requirements, as specified in the RASD, and they will be described in detail.

== Component View

This section defines the static architecture of the system, decomposing it into its main sub-modules. For each component, its responsibilities and the interfaces it uses to interact with other modules are described.

The system is decomposed into three primary software components, as illustrated in @component_diagram. All modules are compiled into a single firmware and run on the device.

#figure(
  caption: [Component View],
  image("diagrams/architecture/components.svg"),
) <component_diagram>

- *UI/Frontend (Qt for MCU):*\
  This component is responsible for rendering the graphical user interface (GUI) on the _Display_ and processing user input from the _Touch Display_ and it is implemented using Qt for MCU.

  It receives high-level commands from the _Backend_ and draws the corresponding screen on the _Display_, and sends to the _Backend_ high-level events after translating them from raw touch data obtained from the _Touch Display_.

- *App logic/Backend (ESP-IDF):*\
  This is the core "brain" of the application. It is written in C++ and contains all business logic. While the final target platform will run on the ESP-IDF framework, the component is developed and tested in a desktop environment using the Qt framework to ensure logic and interfaces are correct before hardware integration.
  It orchestrates the other modules and decides what actions to take:
  - It directly processes the high-level events arriving from the _UI_, giving back a high-level command to update it.
  - It receives raw data from the _Camera_ and passes them to the _AI component_, from which it obtains the embedding of the detected customer.
  - It receives raw data from the _Microphone_ and passes them to the _AI component_, from which it obtains the command to perform.

- *AI component (ESP-DL, ESP-SR):*\
  This specialized module is responsible for executing the pre-trained machine learning models.

  It receives data (images or audio) from the _Backend_, performs inference using ESP-DL and/or ESP-SR runtime, and returns a simple result (e.g., customer embedding or command) to the _Backend_.

== Data Flow Diagram

The following diagram illustrates the data flow between the different layers of the system.

#figure(
  caption: [Data Flow Diagram],
  image("diagrams/architecture/data_flow.png"),
)

== Deployment View

The deployment architecture for this project is a single-node embedded system. All software modules (_Frontend_, _Backend_, and _AI_) are compiled together into a *single monolithic application firmware*. This single firmware binary is then "flashed" onto and executed entirely by the *ESP32-P4-Function-EV* hardware.

#figure(
  caption: [Deployment View],
  image("diagrams/architecture/deployment.svg"),
)

This single-node approach contrasts with typical client-server or cloud-based architectures. By embedding all logic directly into the device, the system guarantees full offline functionality, minimizes latency for real-time interactions (like facial recognition), and ensures user data never leaves the device, providing a strong foundation for privacy.

== Runtime View

In this section, we will illustrate the principal sequences of interactions between the components of the system to accomplish the main tasks of the platform.

=== User Recognition and Menu Display

The _User_ approaches the ECM. The _Backend_ continuously receives images from the _Camera_ and sends them to the _AI component_.

The latter executes a detection model to verify that a face is present in the image and, if it is, where it is located. If and only if a face is detected, the _AI component_ executes another model to effectively recognize the customer and obtain their User ID.

After that, the User ID is sent to the _Backend_, which interrupts the data flow between the _Camera_ and the _AI component_. The _Backend_ compares the User ID received with those present in memory (cache), each of which is associated with a customer already recognized and served.

If a match is found, the _Backend_ updates its internal state by setting the recognized user as the current one. If no match is found, it sets the current user to a guest state (`null`). In both cases, it emits signals (`userChanged`, `beveragesChanged`) to notify the _Frontend_ that its state has changed. The _Frontend_ reacts to these signals by reading the updated properties (the current user and the corresponding beverage list) and refreshing the display to show either a personalized or a guest view.

The _Backend_ waits for events coming from the _Frontend_; if it does not receive them after a certain delay, it communicates to the _Frontend_ to return to the default screen and restarts the data flow between the _Camera_ and the _AI component_.

#figure(
  image("diagrams/sequence_diagrams/user_recognition_and_menu_display.svg"),
)

=== Voice Command Interaction

Immediately after customer recognition, as the Backend sends the menu to the Frontend, it also initiates the voice interaction in parallel.

The _Backend_ begins listening to the _Microphone_ and starts streaming the resulting audio data directly to the _AI component_. The _AI component_ activates and continuously executes a model to detect and recognize voice commands within this live stream.

If a command is recognized, the _AI component_ sends it to the _Backend_, which executes it.

If the _Backend_ doesn't receive any input from either the _AI component_ or the _Frontend_, it stops the audio stream from the _Microphone_ to the _AI component_ and communicates to the _Frontend_ to return to the default screen.

#figure(
  image("diagrams/sequence_diagrams/voice_command_interaction.svg"),
)

=== Drink Customization Through Touch Display

After the customer recognition process is complete and the customer is viewing their menu, they interact with the _Touch Display_ to select one of the proposed beverages.

After the selection, they can request a modification of one or more characteristics of the selected drink.

When they finally give the command to brew their choice, the _Frontend_ calls the `makeDrink` method on the _Backend_, passing the selected `Beverage` object as a parameter..

The latter now begins the process to brew the selected beverage. During this brewing process, the user has the option to cancel the order via the _Touch Display_. If a cancel command is given, the _Frontend_ sends a "cancel event" to the _Backend_, which then aborts the brewing process.

#figure(
  image("diagrams/sequence_diagrams/beverage_selection_and_brewing.svg"),
)

== Component Interfaces

This section defines the formal API and data contracts between the C++ _Backend_ and the QML _Frontend_. The communication is based on the Qt for MCUs property system (`Qul::Property`), which ensures a clean decoupling between logic and presentation while enabling reactive UI updates.

The main elements of the API are:
- *C++ Objects*: Strongly-typed C++ objects exposed to QML (`EdgeCoffeeMachine`, `Beverage`, `User`, `IngredientInfo`)
- *Properties*: Reactive state exposed by the _Backend_ via `Qul::Property`
- *Methods*: C++ functions callable from QML to trigger actions
- *Signals*: Model change notifications (`modelReset()`) for list updates

=== UI/Frontend

The _Frontend_ is responsible for rendering the UI based on the state exposed by the _Backend_'s properties. It triggers actions by calling the _Backend_'s methods in response to user input (e.g., button presses). The _Frontend_ automatically reacts to property changes through QML property bindings.

=== App logic/Backend

The _Backend_ is implemented as a singleton class `EdgeCoffeeMachine` that serves as the central controller. It exposes all state and functionality to QML through a well-defined interface.

==== `EdgeCoffeeMachine`

===== Properties
- `Qul::Property<std::string> status`:
  \ Human-readable status of the machine (e.g., "Idle", "Making Espresso...", "Espresso is ready!", "Welcome `UserName`"). The UI binds to this property and automatically updates when the value changes.

- `Qul::Property<bool> isMakingDrink`:
  \ Boolean flag that is `true` during the brewing process. The UI uses this to show/hide the brewing screen, display progress animations, and disable other interactions during brewing.

- `Qul::Property<Beverage*> selectedBeverage`:
  \ Pointer to the currently selected beverage object. The UI binds to this to display the featured drink's details (name, ingredients, image). Can be `nullptr` if no selection has been made.

- `Qul::Property<User*> user`:
  \ Pointer to the currently logged-in user. It is `nullptr` when in "Guest Mode". The UI binds to this property to switch between guest and personalized views, displaying the user's name in the welcome message.

- `Qul::Property<BeverageModel*> drinksList`:
  \ Pointer to the BeverageModel instance that exposes the beverage list to QML. This model dynamically switches between the global popularity list (guest mode) and the user's personalized list (logged-in mode).

- `Qul::Property<UserModel*> usersList`:
  \ Pointer to the UserModel instance that exposes the list of all registered users. Used by the manual user selection screen.

===== Methods
- `void makeDrink(Beverage* beverage)`:
  \ Initiates the brewing process for the specified beverage. If `beverage` is `nullptr`, uses the `selectedBeverage`. Validates machine state (must be idle), calculates brewing time based on ingredients, sets `isMakingDrink` to `true`, updates `status`, starts the internal brew timer, and automatically logs out the user when brewing completes (via `finishBrewing()`).

- `void stopBrewing()`:
  \ Immediately cancels the active brewing process. Stops the brew timer, sets `isMakingDrink` to `false`, and updates the status message. Called when the user taps the cancel button during brewing.

- `void selectBeverage(Beverage* beverage)`:
  \ Sets the currently selected beverage. Updates the `selectedBeverage` property and the status message (e.g., "Selected: Cappuccino"). For guest users, automatically resets ingredients to defaults.

- `void setUser(User* user)`:
  \ Logs in or logs out a user. When `user` is not `nullptr`, logs in the specified user, updates the welcome message in `status`, and switches `drinksList` to the user's personalized beverage list. When `user` is `nullptr`, logs out the current user and switches back to the global popularity list.

- `void enrollUser(int id)`:
  \ Registers a new user with the specified AI-detected ID. Creates a new `User` object with default settings, adds it to the internal user registry with fast lookup by ID, updates the `usersList` model, and automatically logs in the new user via `setUser()`.

- `void identifyUser(int id)`:
  \ Identifies and logs in an existing user by their AI-detected ID. Looks up the user in the internal registry and calls `setUser()` if found. Logs a warning if the ID is not found.

- `void logoutUser()`:
  \ Logs out the current user by calling `setUser(nullptr)`. Returns the machine to guest mode.

- `Beverage* getSelectedBeverage() const`:
  \ Getter method that returns the currently selected beverage pointer.

- `const std::vector<Beverage*>& getPopularBeverages() const`:
  \ Returns the global list of beverages sorted by popularity. Used internally and by the User class for initialization.

- `bool getIsMakingDrink() const`:
  \ Getter method that returns the brewing state.

- `User* getUser() const`:
  \ Getter method that returns the current user pointer.

==== `Beverage`

The `Beverage` class represents a drink configuration and is exposed to QML as a data object.

===== Properties
- `Qul::Property<std::string> name`: Display name of the beverage
- `Qul::Property<IngredientInfo*> coffeeBeans`: Coffee ingredient configuration
- `Qul::Property<IngredientInfo*> cocoaPowder`: Cocoa ingredient configuration
- `Qul::Property<IngredientInfo*> water`: Water ingredient configuration
- `Qul::Property<IngredientInfo*> foam`: Foam ingredient configuration
- `Qul::Property<IngredientInfo*> milk`: Milk ingredient configuration

===== Methods
- `void resetIngredients()`: Resets all ingredients to their default values. Called from QML when the user taps the reset button in the settings panel.
- `int brewingTime()`: Calculates and returns the estimated brewing time in milliseconds based on current ingredient quantities.

==== `User`

The `User` class represents a user profile with personalized preferences.

===== Properties
- `Qul::Property<std::string> name`: User's display name
- `Qul::Property<std::string> initials`: User's initials (derived from name)
- `Qul::Property<int> picture`: Index of the user's profile picture

===== Methods
- `void beverageBrewed(Beverage* beverage)`: Increments the selection counter, updates the recommendation model weights, adjusts user classification (Default/Conservative/Early Adopter), and refreshes the personalized beverage list.
- `void beverageCustomized()`: Flags the current session as "customized" to boost the `customizerScore` during the next brewing event.
- `const std::vector<Beverage*>& getDisplayBeverages() const`: Returns the user's personalized beverage list, sorted according to their classification category.

==== `IngredientInfo`

The `IngredientInfo` struct defines the configuration for a single ingredient.

===== Properties
- `Qul::Property<float> current`: Current amount (in absolute physical units: grams or milliliters)
- `Qul::Property<float> min`: Minimum allowed amount (absolute units)
- `Qul::Property<float> max`: Maximum allowed amount (absolute units)
- `Qul::Property<float> def`: Default amount (absolute units)

==== List Models (`BeverageModel` & `UserModel`)

Both models inherit from `Qul::ListModel` and provide a reactive interface for QML ListView components.

`BeverageModel`:
- `int count() const`: Returns the number of beverages in the list
- `Beverage* data(int index) const`: Returns the beverage at the specified index
- `void updateList(const std::vector<Beverage*>& newList)`: Updates the model with a new list and emits `modelReset()` to trigger UI refresh

`UserModel`:
- `int count() const`: Returns the number of registered users
- `User* data(int index) const`: Returns the user at the specified index
- `void updateList(const std::vector<User*>& newList)`: Updates the model with a new list and emits `modelReset()` to trigger UI refresh

==== Signals and Reactive Updates

The architecture uses the Qt for MCUs property system for reactive UI updates:

- *Property Bindings*: When QML binds to a `Qul::Property` (e.g., `EdgeCoffeeMachine.status`), the UI automatically updates whenever the C++ code calls `setValue()` on that property.

- *Model Signals*: When `BeverageModel::updateList()` or `UserModel::updateList()` is called, the model emits `modelReset()`, which forces the QML ListView to completely redraw with the new data.

- *Timer Callbacks*: The internal brew timer uses a callback lambda (`m_brewTimer.onTimeout([this]() { finishBrewing(); })`) that is not exposed to QML. When the timer expires, it calls `finishBrewing()`, which updates properties that the UI is bound to.

==== State Diagram: Drink Lifecycle

The following PlantUML diagram illustrates the complete lifecycle of a drink order, from idle state through selection, customization, brewing, and completion:

#figure(
  caption: [Drink Lifecycle State Diagram],
  image("diagrams/drink_lifecycle_state_diagram.svg"),
)

==== QML Binding Examples

The following real snippets from the QML layer show how the UI binds to backend properties and methods.

*Property binding (automatic updates)*
```qml
// Home.qml — featured card updates when the selection changes
Components.FavouritePanel {
  id: favouritePanel
  targetBeverage: EdgeCoffeeMachine.selectedBeverage
}
```

*List model binding and selection*
```qml
// components/DrinkListPanel.qml — ListView bound to drinksList model
ListView {
  id: drinkListView
  model: EdgeCoffeeMachine.drinksList
  delegate: Rectangle {
    // ...
    MouseArea {
      anchors.fill: parent
      onClicked: {
        console.log("Selected drink: " + EdgeCoffeeMachine.drinksList.data(index).name)
        EdgeCoffeeMachine.selectBeverage(EdgeCoffeeMachine.drinksList.data(index))
      }
    }
  }
}
```

*Manual user selection list binding*
```qml
// views/UsersPanel.qml — horizontal user list bound to usersList model
ListView {
  id: userList
  model: EdgeCoffeeMachine.usersList
  orientation: Qt.Horizontal
  delegate: userDelegate
}
```

*Method invocation for drink reset/customization*
```qml
// components/settings/SettingsPanel.qml — reset ingredients from UI
MouseArea {
  anchors.fill: parent
  onClicked: {
    console.log("Reset settings to default for " + root.targetBeverage.name)
    root.targetBeverage.resetIngredients()
  }
}
```

=== AI Component

- `recognize_user(frame: Image): integer`
  \ Method that executes the full face recognition pipeline using `ESP-DL`. It detects if a face is present and compares it against the enrolled database. It returns the `User ID` (positive integer) if the user is recognized, or `-1` if the face is not recognized or not present.
- `get_voice_command(stream: AudioStream): string`
  \ Method that executes a model to detect and recognize a voice command. It returns the string representing the recognized command.

== Selected Architectural Styles and Patterns

The system's architecture is driven by stringent requirements, must be integrated directly into the board, and must be fully functional offline.

=== Modular Monolith

The application is packaged in a single executable (firmware) running on a single processor. However, it is logically decomposed into modules with distinct responsibilities (_UI_, _App logic_, and _AI_) and clear interfaces to ensure maintainability and parallel development.

=== Layered architecture

The software is structured in logical layers:
1. Presentation Layer (_UI/Frontend - Qt for MCU_)
2. Business Logic Layer (_App logic/Backend - C++/Qt, targeting ESP-IDF_)
3. Service Layer (_AI component - ESP-DL/ESP-SR_)

=== UI-Logic Decoupling

A cornerstone of this architecture is the strict decoupling between the Presentation Layer and the Business Logic Layer. This is not an optional preference but a mandatory design choice that directly implements the required system architecture shown in the project's Component View (@component_diagram).

Communication between these two distinct modules is exclusively managed via a formal API:

- *C++ Functions/Properties:*
  \ Exposed by the _Backend_ that the _QML Frontend_ can call to display data or trigger state changes;

- *Qt Signals:*
  \ Emitted by the _Backend_ to notify the _Frontend_ about state changes (e.g., when a drink is ready). The Frontend listens to these signals and updates the UI accordingly.

=== Edge AI

As a core principle of the project, no cloud architecture is used. All data processing (biometrics, voice) and AI model inference occur locally on the device to ensure privacy, low latency, and full offline capability.

This module is not a single library but a composite of two specialized Espressif frameworks:

- *ESP-DL*: For Face detection (FR-F-1) and Customer Recognition (FR-F-2)\
  It is lightweight and general-purpose deep learning library (runtime) optimized for ESP32-P4. We'll execute a pretrained model on the device using the ESP-DL library.

- *ESP-SR*: For Voice Command (FR-V-1)\
  To avoid the highly complex task of training a robust voice command model from scratch and accelerate development, we will adopt ESP-SR (Espressif Speech Recognition). This is a production-ready and highly-optimized framework dedicated specifically to voice applications on their chips. We will need to integrate it and configure it with the activation word and the list of commands specific to our project.

=== Recommendation Engine logic
The _Backend_ implements a dynamic recommendation system based on an exponential decay model.
\ First element of this system is composed by how  beverages are stored. They are stored in a `WeightedSortedList` where the selected drink's weight increases while others decay, ensuring the list evolves with user habits.
\ The second element is the user classification logic. The system tracks two scores, `tryerScore` (variety of drinks chosen) and `customizerScore` (frequency of ingredient modification), to classify users as:

- *Default*: Every user starts in this category. During this initial phase (first 3 interactions), the system prioritizes `Global Popularity` to sort the beverage list, while personal weights are silently initialized in the background.

- *Conservative*: The list is strictly sorted by habit/weight.

- *Early Adopter*: The sorting algorithm intentionally injects less frequent suggestions into top positions to encourage variety.

Each `Beverage` object manages its ingredients via `IngredientInfo` structures that enforce minimum, maximum, and default values in absolute physical units (grams for solids, milliliters for liquids), ensuring valid customization requests.

More details on the data model that supports this logic are provided in the next section.

== Data Model Specification

This section details the system's data structures, including units of measurement, valid ranges, and validation rules.

=== Ingredients and Recipes Data Model

==== Units of Measurement

All ingredient quantities are stored and processed using absolute physical units:

#figure(
  table(
    columns: (auto, auto, auto),
    align: left,
    table.header([*Ingredient*], [*Unit*], [*Physical Meaning*]),
    [Coffee Beans], [grams (g)], [Mass of ground coffee],
    [Cocoa Powder], [grams (g)], [Mass of cocoa powder],
    [Water], [milliliters (ml)], [Volume of water],
    [Foam], [milliliters (ml)], [Volume of milk foam],
    [Milk], [milliliters (ml)], [Volume of liquid milk],
  ),
)

==== Valid Ranges and Defaults

Each ingredient has defined minimum and maximum bounds that reflect both physical constraints (hardware capacity, brewing quality) and recipe standards. These bounds depend on the specific beverage recipe.

#figure(
  table(
    columns: 6,
    align: (left + horizon, center, center, center, center, center),
    table.header(
      [*Beverage*], [*Coffee*\ *(g)*], [*Cocoa*\ *(g)*], [*Water*\ *(ml)*], [*Foam*\ *(ml)*], [*Milk*\ *(ml)*]
    ),

    [*Espresso*], [10.0\ \[7--14\]], [N/A], [30.0\ \[20--50\]], [N/A], [N/A],

    [*Cappuccino*], [9.0\ \[7--14\]], [0.0\ \[0--5\]], [30.0\ \[20--50\]], [50.0\ \[20--80\]], [50.0\ \[0--100\]],

    [*Americano*], [10.0\ \[7--14\]], [N/A], [150.0\ \[100--250\]], [N/A], [N/A],

    [*Latte*], [8.0\ \[7--14\]], [0.0\ \[0--5\]], [30.0\ \[20--50\]], [30.0\ \[0--60\]], [150.0\ \[50--250\]],

    [*Mocha*], [10.0\ \[7--14\]], [10.0\ \[5--10\]], [50.0\ \[30--80\]], [40.0\ \[20--60\]], [100.0\ \[50--200\]],
  ),
)

_Format: Default value \[min--max\] or N/A if the ingredient is not used in the recipe of the corresponsing beverage and its default, min and max are all 0._

*Validation Rules:*

1. *Range Enforcement*: For each ingredient, the constraint $"min" <= "current" <= "max"$ must always hold. Any user modification that violates this constraint is rejected at the UI level (sliders are bound to these ranges).

2. *Non-Negativity*: All ingredient quantities must be non-negative ($>= 0$).

3. *Ingredient Availability*: Users can only modify ingredients that are part of the selected beverage recipe. If a beverage's recipe specifies $"max" = 0$ for a particular ingredient, that ingredient is not available for customization in the UI (FR-D-3).

4. *Default Value Validity*: The default value for each ingredient must satisfy $"min" <= "default" <= "max"$. This is enforced during beverage initialization in `RecipeDatabase`.

5. *Brewing Time Constraint*: The total brewing time calculated from all ingredient quantities must not exceed 180 seconds (NFR-6), which is implicitly guaranteed by the maximum ingredient limits.

=== User Profile Data Model

The `User` class maintains personalization data and behavioral metrics that power the adaptive recommendation engine. Each user maintains an independent weighted list of beverages that evolves based on their selections and customization behavior.

==== Core User Data Fields

#table(
  columns: (auto, auto, auto),
  table.header([*Field*], [*Type*], [*Description*]),
  [`id`], [`int`], [Unique identifier from facial recognition (AI embedding ID)],
  [`name`], [`std::string`], [User's display name],
  [`initials`], [`std::string`], [Derived initials (first and second character if space present)],
  [`picture`], [`int`], [Profile picture index reference],
  [`m_category`], [`UserCategory`], [Classification: Default, Conservative, or EarlyAdopter],
  [`m_numBeverages`], [`int`], [Counter tracking total interactions (used for 3-interaction threshold)],
)

==== Behavioral Scoring System

The recommendation engine uses two accumulated scores that update with each beverage brewing event:

#table(
  columns: (auto, auto, auto),
  table.header([*Score*], [*Range*], [*Purpose*]),
  [`m_tryerScore`],
  [0.0 -- 1.0],
  [Measures tendency to select low-weight (novel) beverages; incremented for variety-seeking],

  [`m_customizerScore`],
  [0.0 -- 1.0],
  [Measures ingredient customization frequency; incremented when user modifies ingredients],

  [`m_customized`], [`bool`], [Session flag indicating ingredients were customized in current session],
)

*Score Update Mathematics:*

When a beverage is brewed, the scores evolve using exponential decay. Let $t$ be the `tryerScore` and $c$ be the `customizerScore`. Then,

$
  t_"new" = r_"trier" times (1 - w) + (1 - r_"trier") times t_"old"
$

where:
- $r_"trier" = 0.25$ (learning rate for trying new drinks)
- $w$ = current normalized weight of the selected beverage (0.0 = novel, 1.0 = favorite)

and

$
  c_"new" = cases(
    r_"customizer" + (1 - r_"customizer") times c_"old" & "if customized",
    (1 - r_"customizer") times c_"old" & "otherwise"
  )
$

where $r_"customizer" = 0.225$ (learning rate for customization events).

==== User Classification Thresholds and Learning Rates

Users are classified into one of three categories based on their behavioral scores, which determines both the display order and the rate at which preferences evolve. Calling $e$ the `earlyAdopterScore` (defined below), the classification rules are:

#table(
  columns: (auto, 7em, 8em, auto),
  table.header([*Category*], [*Activation Condition*], [*Weight\ Learning Rate $r$*], [*Recommendation Behavior*]),

  [Default],
  [$n_"beverages" < 3$],
  [$r_"default" = 0.175$],
  [Display follows global popularity; personal weights silently initialized],

  [Conservative],
  [$n_"beverages" >= 3$ AND $e < 0.5$],
  [$r_"cons." = 0.10$],
  [Display strictly by personal weight (favorites first); slower adaptation],

  [Early Adopter],
  [$n_"beverages" >= 3$ AND $e >= 0.5$],
  [$r_"earlyAd." = 0.25$],
  [Display by personal weight with variety injection; faster adaptation],
)

*Early Adopter Score Calculation:*

After 3 or more beverage selections ($n_"beverages" >= 3$), users are reclassified using a combined `earlyAdopterScore` $e$ defined as:

$
  e = 0.8 times t + 0.2 times c
$

If this score exceeds the threshold of 0.5, the user becomes an Early Adopter; otherwise, they become Conservative. Once classified (at $n_"beverages" = 3$), reclassification occurs at each subsequent beverage selection, allowing users to transition between Conservative and Early Adopter states.

==== WeightedSortedList: Personal Beverage Preferences

Each user maintains a `WeightedSortedList<Beverage*>` that tracks normalized preference weights for every beverage. This list is the core data structure enabling the recommendation engine:

#table(
  columns: (auto, auto, auto),
  table.header([*Component*], [*Type*], [*Description*]),
  [`m_items`], [`std::vector<Beverage*>`], [Ordered list of beverage pointers, sorted by current weight (descending)],
  [`m_weights`], [`std::vector<float>`], [Parallel array of normalized weights, always summing to 1.0],
  [`m_weightR`], [`float`], [Learning rate (0.1--0.25) controlling weight update speed],
)

*Exponential Decay Weight Update Formula:*

Each time a beverage is selected, the weights for the selected beverage and all other beverages are updated as follows:

$
  W_"selected"_"new" & = r + (1 - r) times W_"selected"_"old" \
     W_"other"_"new" & = (1 - r) times W_"other"_"old"
$

where $r$ is the learning rate (`m_weightR`). This ensures:
1. Selected beverage weight increases significantly
2. All other weights decay proportionally
3. Total weight sum remains exactly 1.0 (normalized invariant)
4. The selected item automatically "bubbles up" the sorted list

After each weight update, the internal insertion sort algorithm maintains descending order by weight, reordering items as needed to reflect the new weight distribution.

==== Display Beverage List Generation

The `m_displayBeverages` vector is the ordered list shown to the user in the UI, and its generation depends entirely on the user's classification:

*Default User:* Mirrors the global popularity ordering from `EdgeCoffeeMachine`, mapping global beverage names to the user's local cloned instances. Personal weights are accumulated in the background but not yet used for display.

*Conservative User:* Sorted strictly by personal weight in descending order. All beverages follow their weight ranking, with the highest-weighted at the top.

*Early Adopter User:* Sorted by personal weight (descending), but with a "variety injection" applied: if the list has 4 or more beverages, the lowest-weighted beverage (rarely selected) is removed from the last position and inserted at position 2 (index 2) to encourage exploration of lesser-known drinks.

This three-tier recommendation strategy ensures that:
- New users benefit from global consensus (popular drinks)
- Habitual users see their favorites prominently (familiar drinks)
- Adventurous users are gently nudged toward variety (novel drinks injected at visible position)

== Other design decisions

In agreement with the customer the initial implementation of the system will *not* include a persistent storage. All data generated during the operative time of the system (user registration, user classifications, user preferences) will be held in RAM only. This means that when the device is rebooted, all data except the firmware will be lost.

This decision was made to prioritize the development of the core features, and is based on:
- The high intrinsic complexity of core features like the recommendation engine and the AI-driven interactions;
- The technical risk and uncertainty associated with the implementation of a persistent storage specifically for the ESP32-P4 board, for which the team does not currently have any prior experience.

== Error Handling & Recovery

The system implements error handling for scenarios within the application domain. The following errors are defined, detected, and reported:

=== Handled Error Scenarios

*1. Invalid Beverage Selection*
- *Condition:* User attempts to brew a drink while no valid beverage is selected (e.g., `selectedBeverage` is `nullptr`).
- *Recovery:* The system logs an error message and updates the status to "Invalid beverage."
- *User Message:* "Invalid beverage." (displayed in status field)
- *User Action:* User must select a valid beverage from the list before attempting to brew.
- *Implementation:* Validated in `EdgeCoffeeMachine::makeDrink()`.

*2. Brewing Already in Progress*
- *Condition:* User attempts to initiate a new brew while another drink is being prepared.
- *Recovery:* The system rejects the request and maintains the current brewing state.
- *User Message:* "Already making a drink. Please wait." (displayed in status field)
- *User Action:* User must wait for the current brew to complete or cancel via the "Cancel" button.
- *Implementation:* Guarded by `isMakingDrink` flag in `EdgeCoffeeMachine::makeDrink()`.

*3. User Not Found During Identification*
- *Condition:* The AI subsystem detects a user ID that has not been previously enrolled in the system.
- *Recovery:* The system logs a warning but does not change user state; the machine remains in guest mode.
- *User Message:* No user-facing message; logged at debug level for system monitoring.
- *User Action:* User can enroll as a new user via the "Remember me" flow or continue as a guest.
- *Implementation:* Logged warning in `EdgeCoffeeMachine::identifyUser()`.

*4. Duplicate User Enrollment*
- *Condition:* The AI subsystem attempts to enroll a user ID that already exists in the system.
- *Recovery:* The system rejects the enrollment and logs the attempt.
- *User Message:* No user-facing message; silently prevented by the system.
- *User Action:* System does not allow re-enrollment of the same ID; user is redirected to login flow.
- *Implementation:* Guarded check in `EdgeCoffeeMachine::enrollUser()`.

=== Out-of-Scope Error Scenarios

The following error categories are *out of scope* for the current implementation, as they fall outside the application domain and are handled by hardware/firmware layers:

- *Sensor Faults:* Camera malfunction, microphone failure, or display driver errors are assumed to be handled by the ESP32-P4 board's firmware and platform layer. The application assumes sensors are functional and does not implement fallback logic.
- *Thermal Overshoot:* Heating element thermal runaway, pressure vessel failures, and other hardware safety issues are managed by the physical machine's firmware, not the application.
- *Ingredient Depletion:* The system does not monitor ingredient levels (no sensing hardware is available in the current scope). Operator responsibility to refill supplies.
- *Network Failures:* The application is designed for full offline operation; no cloud connectivity or synchronization is implemented.

== Telemetry & Logging

The system implements structured logging to support development, debugging, and operational monitoring while respecting user privacy.

=== Logging Architecture

The application uses the `Qul::PlatformInterface::log()` function to emit log messages to the platform's logging subsystem. All logs are tagged with a component prefix (e.g., `[ECM]`, `[User]`) for easy filtering and analysis.

*Logging Levels:*
- *Info*: Normal operational events (user login, beverage selection, brew completion)
- *Warning*: Recoverable issues or unexpected state (empty beverage list, unrecognized user ID)
- *Error*: Application-level failures requiring investigation (invalid selections, logic errors)

=== Logged Events

*Backend/Application Logic (`[ECM]`, `[User]`):*
- Machine initialization and default beverage list loading
- User login/logout events (user name logged, not biometric data)
- Beverage selection and weight updates
- Brewing process start, completion, and cancellation
- User enrollment and identification attempts
- Classification transitions (Default → Conservative/Early Adopter)
- Recommendation score calculations (tryer score, customizer score, early adopter score)

=== Privacy & Data Protection

Logging adheres to the following privacy principles:

1. *Non-PII by Default:* User names are logged (as they are user-provided display names), but biometric data (facial embeddings, audio samples) are not logged; only high-level results (user ID or command) are recorded.

2. *Local-Only Logs:* All logs remain on the device in memory or platform-managed buffers. No logs are sent externally.

3. *Session Isolation:* Logs do not persist across device reboots. They are ephemeral and tied to the current session only.

= User Interface Design

== Home View

#figure(
  image("UI/Home - User.png"),
  caption: [
    The coffee machine Home screen for users who have been identified.
  ],
)

When designing the user interface for the coffee machine, our primary goal was to minimize user interactions required to achieve the desired outcome of brewing a beverage. To accomplish this, several user actions were automated and handled by the machine's internal logic.

For example, instead of requiring users to manually initiate facial recognition, the system continuously scans for registered faces. As soon as a known user is detected, the Home View automatically updates with their personalized preferences. Because facial recognition is performed locally (on-premise), this design maintains high privacy and data security for all users.

#figure(
  image("UI/Home - Guest.png"),
  caption: [
    The coffee machine Home screen for guest users.
  ],
)

To handle potential misidentifications, a “Not you?” button is displayed next to the welcome message. This allows quick access to the user list, where one can select a different profile, create a new account, or continue as a guest. Additionally, a "Logout" button is positioned in the top-right corner of the screen to end the current session and return to the guest view. If no user is recognized, the interface displays a generic welcome message, replacing these buttons with a “Remember me” option to enable the registration flow.

Both registered and guest users share a consistent layout. The central area highlights a featured beverage: either the preferred drink of the recognized user or the most popular one for guests. Next to the drink’s image and name, a customization sidebar allows adjustments to parameters such as foam, coffee, milk, water, and powder levels depending on the drink type.

Beneath the customization section, the price and a prominent “Brew Now” button are displayed. On larger screens, an additional sidebar shows a list of available drinks, which can be customized according to the user’s preferences.

#figure(
  image("UI/Home - Small Screens.png"),
  caption: [
    The coffee machine Home screen for small screens.
  ],
)

#figure(
  image("UI/Speech Recognition.png"),
  caption: [
    Speech Recognition indicator.
  ],
)

To enhance interaction transparency during voice input, the bottom area of the interface features a sound wave animation and a real-time transcription of the user's spoken commands. Users can manually activate voice recognition by tapping the waveform. Once a beverage command is recognized, the featured drink updates automatically, and the user can confirm by pressing "Brew Now".

== User Onboarding Views

When a new user signs up, the on boarding process consists of three steps:
1. *Name Input*: The user provides their name or username via voice input. The system confirms recognition accuracy, offering options to retry or continue.
  #figure(
    image("UI/Remember me - 1.png"),
    caption: [
      Onboarding: Name input
    ],
  )
2. *Face Registration*: The system captures facial data and takes a profile picture. The user can retake or confirm the image before proceeding.
  #figure(
    image("UI/Remember me - 2.png"),
    caption: [
      Onboarding: Face registration
    ],
  )
3. *Welcome Screen*: The process concludes with a confirmation message, informing the user that they will now be automatically recognized by the machine in future sessions.
  #figure(
    image("UI/Remember me - 3.png"),
    caption: [
      Onboarding: Welcome screen
    ],
  )

== Manual User Selection View

#figure(
  image("UI/Manual User Selection.png"),
  caption: [
    Manual user selection view
  ],
)

If the machine fails to identify a user, the *Manual User Selection View* allows them to choose a profile manually. This view displays all existing user profiles, along with a Guest option and the ability to add a new user.

== Drinks List View

#figure(
  image("UI/Drinks list.png"),
  caption: [
    Drinks list view
  ],
)

On smaller displays, where space is limited, the full list of beverages is not shown on the *Home View*. By selecting the "All Drinks" button, users can browse the complete catalog of beverages, each accompanied by pricing information and grouped by drink category.

== Brewing View

#figure(
  image("UI/Home - Brewing.png"),
  caption: [
    Home: Brewing view
  ],
)

When the brewing process begins, the user remains on the Home View. To draw focus, the rest of the interface is dimmed and slightly blurred, while the price component remains visible.

During brewing, the price area transforms into a progress indicator, displaying a cancel button and an animated wave that gradually rises as brewing progresses, reaching the top upon completion.

#figure(
  rect(image("UI/Color Palette.png", width: 100%), inset: 0pt, stroke: 3pt),
  caption: [
    The color palette.
  ],
) <color_palette_ref>

These designs will be iterated based on customer feedback throughout the development process and the feasibility of the implementation using the Qt Framework.

= Implementation and Testing Plan

The project's initial phase was defined by a key technical challenge: the official Qt for MCU support package for the ESP32-P4-Function-EV board was not yet available. To mitigate this dependency, the team adopted a parallel development strategy, splitting the project into two independent tracks:

1. *Track 1: Desktop UI Application (UI + Qt-enabled Backend)*\
  This track focused on developing the complete user experience on a standard desktop environment. It involved creating the QML-based user interface and integrating it with a version of the C++ backend that utilizes Qt's meta-object system. This allowed for rapid prototyping and testing of all UI-related features.

2. *Track 2: On-Board AI and Logic Integration (AI + Core Backend)*\
  This track focused on the core embedded functionalities on the ESP32-P4 board. A version of the C++ backend logic, stripped of Qt dependencies, was integrated directly with the AI-driven face recognition application, proving the viability of the core on-device features.

Recently, the board support package has been provided by the customer. The team has therefore moved into the next phase: integrating the two development tracks into a single, unified application running on the target hardware. This integration process is currently underway and represents the main focus of the development effort, as the team works to resolve the complexities of merging the Qt-based UI with the on-board AI and logic components.

== Testing Strategy Overview

The testing approach follows a three-layer pyramid: *unit tests* for core business logic, *UI integration tests* for QML/C++ interactions, and *hardware-in-the-loop (HIL) tests* for real-world device behavior. This strategy ensures comprehensive validation of all functional and non-functional requirements against acceptance criteria.

=== Unit Tests

The following unit tests are implemented in `tests/` using Catch2.

==== Beverage Model Tests

The `TestBeverage.cpp` file validates recipe data and ingredient customization logic:

- *Initialization*: Verifies all ingredients are correctly initialized with current, default, min, and max values.
- *Brewing Time Calculation*: Validates the formula `brewingTime() = sum(0.5 + normalized[i] * 10.0) * 1000 ms` across multiple ingredient configurations.
- *Reset Functionality*: Confirms `resetIngredients()` restores all values to defaults (FR-D-5).
- *Ingredient Bounds*: Ensures normalization respects min ≤ current ≤ max constraints (FR-D-3).

*Acceptance Criteria:* Each beverage must enforce ingredient ranges and calculate accurate brewing times ≤ 180 seconds (NFR-6).

==== User Classification & Recommendation Tests

The `TestUser.cpp` file validates user profile logic and personalization algorithms:

- *Default Category*: New users start in the "Default" category, shown global popularity list (FR-U-3).
- *Conservative Classification*: After ≥3 similar beverage selections with little customization, user is classified as "Conservative" and shown his favorites first (FR-U-4).
- *Early Adopter Classification*: High variety (different drinks) + high customization (ingredient modifications) → "EarlyAdopter" category; untried drink injected into top 3 recommendations (FR-U-5).
- *History Tracking*: `beverageBrewed()` increments drink counters; `beverageCustomized()` flags session for customization scoring (FR-S-1).
- *Error Resilience*: Null or unknown beverages handled gracefully without state corruption.

*Acceptance Criteria:* Classification must accurately reflect user behavior; recommendation list must be reordered correctly after each brew.

==== WeightedSortedList (Recommendation Algorithm) Tests

The `TestWeightedSortedList.cpp` file validates the exponential decay recommendation engine:

- *Uniform Initialization*: All items start with equal weight = 1.0 / count.
- *Weight Normalization*: Sum of all weights ≈ 1.0 (within float epsilon) after any operation.
- *Selection Boost*: When item is selected, its weight is $W = R + (1 − R) × W_"old"$; others decay to $(1 − R) × W_"old"$ (where $R$ is the learning rate).
- *Reordering*: Items with higher weights automatically sort to the front of the list.
- *Configurable Learning Rate*: Different rates ($0.2$, $0.3$, $0.5$) produce expected weight deltas.
- *Edge Cases*: Empty lists handled safely.

*Acceptance Criteria:* Weights must remain normalized and sorted correctly across all operations. Learning rate must tune the speed of preference decay.

==== EdgeCoffeeMachine Core Logic Tests

The `TestEdgeCoffeeMachine.cpp` file validates the state machine and main controller:

- *Initialization*: Machine starts in "Idle" state with `isMakingDrink = false`, default beverage selected, no user logged in.
- *Beverage Selection*: `selectBeverage()` updates `selectedBeverage` property and `status` message (e.g., "Selected: Cappuccino").
- *Brewing State Transition*: `makeDrink()` sets `isMakingDrink = true` and updates status; `stopBrewing()` cancels and returns to idle.
- *User Session*: `setUser(user)` logs in; `logoutUser()` clears current user and reverts to guest menu (FR-UI-4).
- *Error Handling*: Attempting to brew without selection returns error status "Invalid beverage." (NFR-7).

*Acceptance Criteria:* All state transitions must be atomic and leave the machine in a consistent state.

=== UI Integration Tests

UI integration tests will validate the interaction between QML frontend and C++ backend on a desktop environment. Mock implementations will substitute the AI module (face recognition, voice commands) and brew timer for deterministic, repeatable testing. Since Qt for MCU does not provide UI testing support for the ESP32-P4-Function-EV board, a golden reference approach will be used for these tests. Consequently, only backend changes that are reflected in the user interface will be validated.

==== Touch Navigation & Ingredient Customization

- *Drink List Binding*: QML ListView displays all beverages from `BeverageModel`; selecting a drink calls `EdgeCoffeeMachine::selectBeverage()`.
- *Ingredient Sliders*: Dragging a slider updates the corresponding `IngredientInfo::current` value; value is bounded by min/max.
- *Reset Button*: Tapping the reset button invokes `Beverage::resetIngredients()` (FR-D-5).

==== Recognition & Status Feedback

- *User Recognition Indication*: When a user is logged in, the UI displays their name and a welcome message (FR-UI-1).
- *Guest Mode Visual*: When in guest mode, the UI shows "Popular Drinks" and a "Remember me" button (FR-UI-1).

==== Brewing Display & Cancellation

- *Brew Screen Transition*: After `makeDrink()` is called, the UI transitions to a brewing screen (FR-UI-2).
- *Progress Animation*: A visual progress indicator (bar or waveform) animates during brewing; updates at least every 500 ms.
- *Cancel Button*: User can tap a cancel button during brewing, which calls `EdgeCoffeeMachine::stopBrewing()` (FR-UI-3).

==== Post-Brewing Reset

- *Idle Transition*: After brewing completes or is cancelled, the UI returns to the idle/recognition screen (FR-UI-4).
- *Auto-Logout*: The current user is logged out (`logoutUser()` called) after brewing finishes (FR-UI-4).
- *Ready for Next User*: System is ready to detect a new user immediately after; no stale state persists.

=== Hardware-in-the-Loop Tests

HIL tests run the complete firmware on the actual ESP32-P4-Function-EV board, validating real-time performance, AI inference, and device integration.

==== Facial Recognition Pipeline

- *Face Detection Latency*: On actual camera input, face detection completes within 2 seconds.
- *User Recognition*: Detected face is recognized and a valid user ID (≥ 1) is returned, or −1 if unknown (FR-F-2).
- *Confidence Threshold*: Faces with $<60%$ confidence are rejected; the system falls back to guest mode (FR-F-3).
- *Recognition Timeout*: Abort recognition after 10 seconds; gracefully return −1 (FR-F-4).
- *User Enrollment*: Unrecognized faces can be enrolled; a new `User` object is created with an assigned ID.

*Performance Metrics:*
- End-to-end recognition + menu display ≤ 10 seconds (NFR-1).
- Face detection ≤ 2 seconds.

==== Voice Command Pipeline

- *Command Recognition*: Spoken commands are detected and recognized; the system returns a command string (e.g., "make_espresso") (FR-V-1).
- *Confidence Threshold*: Commands with $<60%$ confidence are rejected; the UI shows "Didn't catch that" and offers retry (FR-V-2).
- *Audio Timeout*: Audio capture stops after 5 seconds of audio or 2 seconds of silence; the UI shows a retry prompt (FR-V-3).
- *Graceful Timeout*: Timeout does not crash the system; recovery to idle state completes within 5 seconds (NFR-7).

*Performance Metrics:*
- Voice command latency ≤ 3 seconds (from end of utterance to recognition result).

==== UI Responsiveness & Frame Rate

- *Touch Response*: Touch input registers within 100 ms; visual feedback (button press effect) appears within 100 ms (NFR-5).
- *Screen Transitions*: Menu updates and view transitions complete within 1500 ms (NFR-2).
- *Frame Rate*: UI renders at ≥ 20 FPS under nominal load (NFR-2).

==== End-to-End User Scenarios

Three complete user journeys are validated on hardware:

*Scenario S1 – First-Time User:*
1. User approaches machine; face is detected and enrolled.
2. User is shown default menu (FR-U-3); selects a drink.
3. User customizes ingredients (FR-D-2); selects within bounds (FR-D-3).
4. User issues voice command "Make espresso" (FR-V-1); command is recognized.
5. Brewing starts; progress displayed (FR-UI-2).
6. Brewing completes in ≤ 180 seconds (NFR-6).
7. System resets to idle within 5 seconds; ready for next user (FR-UI-4, NFR-7).

*Scenario S2 – Returning Conservative User:*
1. Recognized user's face triggers personalized menu (FR-F-2).
2. User is classified as "Conservative" (FR-U-4); top 2 favorites displayed first (FR-U-4).
3. User selects their favorite without customization.
4. Brewing starts via touch; completes; system resets.

*Scenario S3 – Early Adopter with Customization & Cancellation:*
1. Recognized user shown "EarlyAdopter" menu with novel drinks injected (FR-U-5).
2. User selects an untried drink and customizes ingredients heavily (FR-D-2).
3. User cancels mid-brew by tapping cancel button (FR-UI-3).
4. System stops brewing and returns to idle within 5 seconds (NFR-7).

=== Requirements Mapping

The highlights below provide traceability from each requirement to its corresponding unit, integration, or HIL test.

- *FR-D-1 to FR-D-5* (Drink Catalogue & Customization): Unit tests validate; integration & HIL tests validate UI binding.
- *FR-U-2 to FR-U-5* (User Classification & Menus): Unit tests validate classification logic; HIL tests validate real-world recognition.
- *FR-F-1 to FR-F-4* (Face Detection & Recognition): HIL tests validate; unit tests mock AI responses.
- *FR-V-1 to FR-V-3* (Voice Commands): HIL tests validate; integration tests mock voice results.
- *FR-T-1* (Touch Navigation): Integration tests validate QML binding; HIL tests validate hardware responsiveness.
- *FR-UI-1 to FR-UI-4* (UI Feedback & Lifecycle): Integration tests validate property binding; HIL tests validate real-time display.
- *NFR-1 to NFR-9* (Performance, Privacy, Reliability): HIL tests measure real-world timing; integration tests validate under mock load.

= Glossary

== Acronyms

- *ECM*: Edge Coffee Machine.
- *UI*: User Interface.
- *UX*: User Experience.
- *MCU*: Microcontroller Unit.
- *AI*: Artificial Intelligence.
- *HIL*: Hardware-in-the-Loop.
- *FPS*: Frames Per Second.
- *RASD*: Requirements Analysis and Specification Document.
- *GUI*: Graphical User Interface

#bibliography("design_references.yaml", full: true, title: "Design References")
