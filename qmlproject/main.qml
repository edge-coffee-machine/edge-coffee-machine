import QtQuick 2.0
import QtQuick.Controls 2.15
import "user-interface/src/components"
import "user-interface/src/views"


Item {
    width: 1024; height: 600

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