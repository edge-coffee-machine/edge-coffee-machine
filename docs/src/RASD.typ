#set document(
  title: [Requirements Analysis and Specification Document],
  keywords: ("requirements analysis", "specification"),
)

#set text(size: 13.3pt)
#set page(
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
#show outline.entry.where(level: 1): it => {
  v(13pt, weak: true)
  strong(it)
}
#show outline.entry.where(level: 3): it => {
  text(style: "italic", it)
}
#show heading.where(level: 4): it => text(
  weight: "regular",
  style: "italic",
  it.body + [.],
)
#show title: set text(size: 36pt, weight: 500)
#show link: it => text(
  fill: rgb(blue),
  it,
)
#show "-th": "\u{2011}th" // Non-breaking hyphen
#show "C++": name => box(name) // Ensure the + sign isn't split across lines
#show "high-priority-sym": "!!!"
#show "medium-priority-sym": "!!"
#show "low-priority-sym": "!"

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

// Title and authors
#align(
  center,
  {
    title()
    text(size: 24pt, fill: rgb("#777"))[Edge Coffee Machine]
    linebreak()
    v(4pt)
    text(size: 1.2em)[Version 1.3 · 2025-12-21]
    linebreak()
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

The primary purpose of this document is to formally define the functional and non-functional requirements for the Edge Coffee Machine application, a smart coffee machine featuring Edge AI. This specification will serve as the guiding blueprint for the development team, detailing the required user interface, business logic and AI components. The RASD focuses strictly on what the application must do, and deliberately avoids detailed discussion of how the requirements will be implemented. It ensures all stakeholders, including the development team, coordinators, and the project customer, have a clear understanding of the final product to be implemented.

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

This requirements specification covers the development of a fully functional application for the Edge Coffee Machine. The scope encompasses:

- User Interface: Creating a fluid, high-performance interface that handles all user interactions (touchscreen, camera image recognition, and voice commands).

- Application Logic: Implementing the core business logic: mainly customer classification and personalized drink suggestions.

- AI Component: Integrating AI models for image recognition and voice commands on the target microcontroller.

The integration with the physical coffee machine hardware and the handling of payments are out of scope.

== Revision History
- 2025-11-06: 1.0: Initial version
- 2025-11-23: 1.1:
  - Split the "Personalized Drink Menu" requirement in 3: "Default Customers Menu", "Conservative Customers Menu", and "Early Adopter Menu"
  - Add the requirement "Reset default ingredients"
- 2025-12-07: 1.2:
  - Add personas
  - Add success metrics
  - Add constraints derived from the board components' data-sheets
- 2025-12-21: 1.3:
  - Add requirements prioritization
  - Change requirement FR-D-1 to offer at least 5 drinks instead of 10 following meeting with the customer
  - Specialize privacy non-functional requirements
  - Add ingredient range constraints (add FR-D-3, update FR-D-2 description)
  - Make layout adjustments

= Requirements Definition

== High-Level Description

=== System Characteristics

The Edge Coffee Machine application is designed to run on the ESP32-P4-Function-EV microcontroller board. This is a crucial detail, as it defines the embedded system constraints for the entire project.

The system's operation relies on the following key I/O components:

- Camera: Used as the input for the face detection and recognition AI models, primarily for automatic customer classification.

- Microphone: Used as the input for the voice commands AI model, allowing users to verbally initiate the brewing process.

- LCD Touchscreen: Provides the primary interactive display for menu browsing and drink customization.

The microcontroller does not include persistent storage, so data is lost between sessions. There is, however, a microSD card slot that could be used for persistent storage in future development stages.

=== User Characteristics

The target users for the Edge Coffee Machine are office workers in a typical corporate or shared workspace environment. Their primary interaction characteristics can be summarized as:

- Seamless experience: Users typically seek a simple, quick, and efficient process to obtain their preferred coffee beverage with minimal friction.

- Personalization: Users expect to benefit from the system's ability to recognize them and present a personalized menu.

- Drink customization: Users can customize their drinks and brewing recipes.

The system categorizes users into three distinct profiles, which drive the personalized drink menu and experience:

#table(
  columns: 3,
  table.header([*Category*], [*Description*], [*Menu Strategy*]),
  [Default customers],
  [
    A user the system doesn't know enough about or an individual the system has failed to recognize.
  ],
  [
    Present a standard menu of popular drinks and basic coffee options.
  ],

  [Conservatives],
  [
    A recognized user whose historical data shows they rarely deviate from one or two drinks and don't customize it.
  ],
  [
    Prominently display their favorite drink or two.
  ],

  [Early adopters],
  [
    A recognized user whose historical data suggests they frequently try new drinks and  customize them.
  ],
  [
    Highlight new beverages, and offer customization options.
  ],
)

=== Personas

The following personas represent key user types, each with specific goals, pain points, and key performance indicators (KPIs) to measure success. Maintenance staff and other non-user roles are not included, as they are out of scope for the current project in accordance with the customer's guidelines.

#table(
  columns: (5em, auto, auto, auto),
  table.header([*Persona*], [*Goals*], [*Pain points*], [*KPIs*]),
  [Accessibility user],
  [
    Operate the ECM without fine motor control or full vision.
  ],
  [
    Small touch targets, low contrast UI elements.
  ],
  [
    80% successful orders via voice; contrast ratio for text elements ≥~4.5:1.
  ],

  [First-time user],
  [
    Complete an order without prior knowledge; understand available controls quickly.
  ],
  [
    UI views hidden under buttons that lead to unclear navigation.
  ],
  [
    Time to first successful brew ≤~90~s; customize drink without help ≥~90% of the time.
  ],

  [Busy-hour user],
  [
    Obtain coffee rapidly during peak traffic with minimal interaction steps.
  ],
  [
    Long queues, excessive customization steps.
  ],
  [
    Median time from detection to brew start <~30~s during peak; customization settings load in <~5~s.
  ],
)

=== Success Metrics

To evaluate the Edge Coffee Machine's performance and user satisfaction, the following measurable success metrics will be tracked:

#table(
  columns: (6em, auto, auto),
  table.header([*Metric*], [*Description*], [*Target Value*]),
  [Throughput],
  [
    Number of drinks successfully brewed per hour during peak operation.
  ],
  [
    ≥~20 drinks/hour
  ],

  [Average wait time],
  [
    Mean time from user face detection to brew start (including recognition, menu navigation, and drink selection).
  ],
  [
    ≤~30 seconds
  ],

  [Recognition accuracy],
  [
    Percentage of users correctly recognized by the facial recognition system.
  ],
  [
    ≥~80%
  ],

  [Voice command success rate],
  [
    Percentage of voice commands correctly interpreted and executed on first attempt.
  ],
  [
    ≥~80%
  ],
)

=== Functionalities Overview

Here is a high-level overview of the main functionalities that the Edge Coffee Machine will provide:
- *Interactive Interface:* The LCD touchscreen acts as the main interface for all interactions, enabling users to browse, select, and customize drinks through a graphical user interface.
- *Facial Recognition:* The system automatically detects and recognizes users through the integrated camera using a local face recognition model. Based on past interactions, users are classified into categories (Default customer, Conservative, or Early adopter), which influence drink recommendations.
- *Voice Commands:* Users can interact with the coffee machine using specific voice commands processed locally by a speech recognition model. This enables hands-free operation, such as starting the brewing process or selecting a preferred drink.
- *Personalized Drink Recommendation and Customization:* Recognized users are presented with personalized drinks tailored to their preferences and usage patterns. Users can also manually adjust drink parameters through the touchscreen interface.

== Requirements and User Stories

The Source column indicates whether the requirement is clearly stated in the project guidelines provided by The Qt Group or is an interpretation made by the development team. The Prio column indicates the priority level assigned to each requirement: high (high-priority-sym), medium (medium-priority-sym), or low (low-priority-sym).

=== Functional Requirements

#table(
  columns: (auto, 6.5em, auto, 2.5em, auto),
  align: (center, left, left, center, center),
  table.header([*ID*], [*Name*], [*Description*], [*Source*], [*Prio*]),
  // FACE DETECTION
  [FR-F-1],
  [Face detection],
  [When the system is in idle state, it shall monitor the camera input to detect the presence of a user and trigger the recognition process. During an active user session, face detection shall be paused.],
  [Qt],
  [medium-priority-sym],

  // FACIAL RECOGNITION
  [FR-F-2],
  [Facial recognition],
  [Once the integrated camera has detected the presence of a user, the system shall recognize their identity through a facial recognition model.],
  [Qt],
  [high-priority-sym],

  [FR-F-3],
  [Recognition confidence],
  [A user shall be identified only when the face recognition confidence is greater than 60%. Below this threshold, the UI shall silently fall back to the popular-drinks menu as the user can always manually switch profile.],
  [Team],
  [medium-priority-sym],

  [FR-F-4],
  [Recognition timeout],
  [Each recognition attempt shall time out after 10 seconds. On timeout, the UI shall silently fall back to the popular-drinks menu as the user can always manually switch profile.],
  [Team],
  [medium-priority-sym],

  // USER
  [FR-U-1],
  [Unrecognized user handling],
  [An unrecognized user can still use the machine. Since there is no profile associated, the list of most popular beverages will be shown.],
  [Qt],
  [high-priority-sym],

  [FR-U-2],
  [User classification],
  [Recognized users must be classified into one of three categories (Default customers, Conservatives, Early adopters) based on interaction history.],
  [Qt],
  [high-priority-sym],

  [FR-U-3],
  [Default customers menu],
  [Default customers will be shown the list of the most popular drinks.],
  [Qt],
  [high-priority-sym],

  [FR-U-4],
  [Conservative customers menu],
  [Conservative customers will be shown their 2 preferred drinks at the start of the drinks list.],
  [Qt],
  [high-priority-sym],

  [FR-U-5],
  [Early adopter customers menu],
  [Early adopter customers will be shown drinks they have not tried yet at the start of the drinks list.],
  [Qt],
  [high-priority-sym],

  // DRINKS
  [FR-D-1],
  [Drink catalogue],
  [The system shall offer at least 5 different drinks, each one defined by a distinct graphic and default ingredients: amount of coffee beans, cocoa powder, water, foam and milk. The most prominent drinks shown depend on the category of the customer and their previous selections.],
  [Qt],
  [high-priority-sym],

  [FR-D-2],
  [Adjust drink ingredients],
  [The user can adjust the amount of ingredients for the selected drink within predefined minimum and maximum bounds specific to each ingredient type.],
  [Qt],
  [medium-priority-sym],

  [FR-D-3],
  [Ingredient range constraints],
  [Each ingredient shall have defined minimum and maximum limits. The system shall prevent users from selecting ingredient quantities outside these validated ranges.],
  [Team],
  [medium-priority-sym],

  [FR-D-4],
  [Ingredient availability constraint],
  [The user can only adjust the amount of ingredients that are part of the selected drink. If the chosen drink does not include some ingredients, those quantities shall not be available for modification.],
  [Qt],
  [medium-priority-sym],

  [FR-D-5],
  [Reset default ingredients],
  [The user can use a button to reset a drink's ingredient to their default values.],
  [Qt],
  [low-priority-sym],

  // VOICE
  [FR-V-1],
  [Voice commands],
  [The user can start the brewing process using voice commands. These commands are processed locally by a speech recognition model.],
  [Team],
  [medium-priority-sym],

  // TODO: check confidence threshold value
  [FR-V-2],
  [Voice confidence],
  [A voice command shall execute only if the top intent confidence is greater than 60%; otherwise, the UI shall show a "Didn't catch that" prompt, keep the machine idle, and offer one-tap retry or touch navigation.],
  [Team],
  [medium-priority-sym],

  [FR-V-3],
  [Voice timeout],
  [Voice capture windows shall auto-close after 5 seconds of audio or 2 seconds of silence. On timeout, the UI shall display a retry prompt and retain the current screen.],
  [Team],
  [medium-priority-sym],

  // TOUCHSCREEN
  [FR-T-1],
  [Touch-based navigation],
  [The user must be able to use the touchscreen to navigate through the interface and use its functionalities: drink selection, ingredient customization, start/interrupt brewing, and logging out.],
  [Team],
  [high-priority-sym],

  // UI
  [FR-UI-1],
  [Recognition feedback],
  [The user interface shall display a clear visual indication showing whether the facial recognition system has successfully recognized the user.],
  [Team],
  [medium-priority-sym],

  [FR-UI-2],
  [Brewing process display],
  [After the user selects a drink and starts the brewing process, the user interface shall display a visual representation of the brewing process, with a duration according to the drink and any ingredient customization.],
  [Qt],
  [low-priority-sym],

  [FR-UI-3], [Brewing interruption], [The user must be able to cancel the brewing process.], [Qt], [low-priority-sym],

  [FR-UI-4],
  [Post-Brewing Reset],
  [When the brewing process ends, even if it is interrupted, the system should wait again for another user to be recognized.],
  [Qt],
  [high-priority-sym],

  // SYSTEM
  [FR-S-1],
  [User History],
  [The system shall save each user's drink selection in order to generate a customized set of drinks based on previous interactions.],
  [Qt],
  [high-priority-sym],
)

=== Non-Functional Requirements

#table(
  columns: (auto, 6.5em, auto, 2.5em, auto),
  align: (center, left, left, center, center),
  table.header([*ID*], [*Name*], [*Description*], [*Source*], [*Prio*]),
  [NFR-1],
  [Fast performance],
  [The user recognition must return a result in maximum 10 seconds to ensure a fluid user experience.],
  [Team],
  [high-priority-sym],

  [NFR-2],
  [UI/UX quality],
  [The user interface should not have a delay greater than 5 seconds and it should render at a minimum frame rate of 20 FPS under nominal operating conditions.],
  [Team],
  [high-priority-sym],

  [NFR-3],
  [Security],
  [The system shall operate entirely offline to eliminate exposure to network-based security threats.],
  [Qt],
  [high-priority-sym],

  [NFR-4],
  [Privacy],
  [All processing and data storage shall occur locally on the device to protect user data from transmission or interception.],
  [Qt],
  [high-priority-sym],

  [NFR-4.1],
  [Biometric privacy],
  [Face embeddings and recognition inference shall remain on-device; no biometric images or vectors shall be transmitted off the MCU.],
  [Team],
  [high-priority-sym],

  [NFR-4.2],
  [Voice privacy],
  [Voice processing remains fully on-device; no audio is sent off the MCU and no recordings persist beyond the active session.],
  [Team],
  [high-priority-sym],

  [NFR-5],
  [UI responsiveness],
  [Touch and button interactions shall provide visual feedback within 100 ms. Screen transitions and menu updates shall complete within 1500 ms under nominal load.],
  [Team],
  [medium-priority-sym],

  [NFR-6],
  [Brew time bounds],
  [The brewing process shall not exceed 180 seconds for any drink configuration.],
  [Team],
  [medium-priority-sym],

  [NFR-7],
  [Error recovery],
  [Upon encountering a recoverable error, the system shall return to a usable idle state within 5 seconds. Critical errors shall trigger an automatic restart within 10 seconds.],
  [Team],
  [high-priority-sym],

  [NFR-8],
  [Reliability],
  [The system shall maintain stable operation with a Mean Time Between Failures (MTBF) of at least 100 hours of continuous use under nominal operating conditions.],
  [Team],
  [medium-priority-sym],

  [NFR-9],
  [Session logging],
  [The system may retain interaction logs (user selections, timestamps, errors) for the current session in volatile memory only, which will be cleared upon power cycle.],
  [Team],
  [low-priority-sym],
)

=== Constraints

Constraint 2--5 are derived from the target hardware, consulting the ESP32-P4-Function-EV components' datasheets.

#table(
  columns: (auto, 6.5em, auto, auto),
  align: (center, left, left, center),
  table.header([*ID*], [*Name*], [*Description*], [*Source*]),
  [C-1],
  [Platform compliance],
  [The application must be implemented using C++ with the ESP-IDF framework and Qt for MCU. It must be deployable on the ESP32-P4-Function-EV board.],
  [Qt],

  [C-2],
  [Operating Temperature],
  [The system must operate reliably within the temperature range of 0°C to 60°C.],
  [Datasheet],

  [C-3],
  [Humidity],
  [The system must operate reliably with a relative humidity smaller than 85% non-condensing.],
  [Datasheet],

  [C-4],
  [Backlight Power],
  [The system must supply a constant current of 180~mA at 9.0~V--10.8~V to the display backlight driver.],
  [Datasheet],

  [C-5],
  [UI Active Area],
  [The Graphical User Interface must be designed to fit strictly within the active display area of 154.21~mm×85.92~mm.],
  [Datasheet],

  [C-6], [Binary Size], [The built binary must be under 16MB of total size.], [Datasheet],
)

=== User Stories

#[
  #set par(first-line-indent: 0em)

  *US1 – Automatic User Recognition*

  As a user, I want the machine to automatically detect and recognize my face when I approach so that the system immediately prepares a personalized menu.

  *Associated requirements:* FR-F-1, FR-F-2, FR-F-3, FR-F-4, FR-UI-1, NFR-4.1.

  *US2 – Personalized Menu Recommendation*

  As a recognized user, I want the system to dynamically classify me based on my history and prioritize drink recommendations accordingly so that I can quickly select a relevant beverage.

  *Associated requirements:* FR-U-2, FR-U-3, FR-U-4, FR-U-5, FR-D-1, FR-S-1.

  *US3 – Recognized User Access*

  As a user, I want the interface to clearly indicate if I have been successfully recognized so that I know which type of menu (personalized or default) to expect.

  *Associated requirements:* FR-UI-1.

  *US4 – Default Drink Catalogue*

  As a user, I want the system to offer a catalogue of at least 10 different drinks so that I have a variety of choices.

  *Associated requirements:* FR-D-1.

  *US5 – Full Touchscreen Control*

  As a user, I want the touchscreen to be the main point of control for all features (selection, customization, start, cancel) so that the machine is intuitive and easy to navigate.

  *Associated requirements:* FR-T-1.

  *US6 – Voice Command Brewing*

  As a user, I want to be able to confirm and start the brewing process using a voice command so that I can initiate my order hands-free.

  *Associated requirements:* FR-V-1, FR-V-2, FR-V-3, NFR-4.2.

  *US7 – Brewing Status and Interruption*

  As a user, I want the interface to clearly display my brewing progress and allow me to cancel the cycle at any point.

  *Associated requirements:* FR-UI-2, FR-UI-3.

  *US8 – Session Reset*

  As a subsequent user, I want the machine to automatically reset to the recognition state after the previous user finishes or cancels their order so that I can immediately start my session.

  *Associated requirements:* FR-UI-4.

  *US9 – Ingredient Customization*

  As a user, I want to be able to access and precisely modify the amount of ingredients for my selected drink.

  *Associated requirements:* FR-D-2, FR-D-3, FR-D-4.
]

=== Use Case Diagram

#figure(
  caption: [Use Case Diagram],
  image("diagrams/use_case_diagram.png"),
)

= Glossary

== Acronyms

- *AI*: Artificial Intelligence
- *DSD*: Distributed Software Development
- *ECM*: Edge Coffee Machine
- *MCU*: Microcontroller Unit
- *RASD*: Requirements Analysis and Specification Document
- *UI*: User Interface
- *UX*: User Experience
