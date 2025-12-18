import QtQuick 2.0
import QtQuick.Controls 2.15
import "user-interface/src/components"
import "user-interface/src/views"

Item {
    height: 600
    width: 1024

    // Frame rate limiter - reduces rendering to 30 FPS
    Timer {
        id: frameRateLimiter

        property int frameCount: 0

        interval: 33  // ~30 FPS (1000ms / 33ms ≈ 30 frames)
        repeat: true
        running: true

        onTriggered: {
            // Force a frame update at controlled intervals
            frameCount++;
        }
    }

    SwipeView {
        id: mainSwipeView

        anchors.fill: parent
        interactive: false // Disable user swiping; navigation controlled programmatically

        // Page 1: Home
        Home {
            onDrinksListRequested: mainSwipeView.currentIndex = 2 // Navigate to Drinks List page
            onSwitchUserRequested: mainSwipeView.currentIndex = 1 // Navigate to Users page
        }

        // Page 2: Users Panel
        UsersPanel {
            onGoBackRequested: mainSwipeView.currentIndex = 0 // Navigate back to Home page
        }

        // Page 3: Drinks List
        DrinksList {
            onGoBackRequested: mainSwipeView.currentIndex = 0 // Navigate back to Home page
            onSwitchUserRequested: mainSwipeView.currentIndex = 1 // Navigate to Users page
        }
    }
}
