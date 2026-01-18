// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include <platform/singlepointtoucheventdispatcher.h>
#include <platforminterface/platforminterface.h>

namespace Qul {
namespace Platform {

/*!
    \ingroup platformapi
    \headerfile <platform/singlepointtoucheventdispatcher.h>

    \title Single point touch event dispatcher

    \brief A dispatcher for single point touch events.

    \section1 Classes
    \annotatedlist header-singlepointtoucheventdispatcher
*/

/*!
    \class Qul::Platform::SinglePointTouchEvent
    \relatedheader singlepointtoucheventdispatcher
    \inheaderfile platform/singlepointtoucheventdispatcher.h
    \brief A struct containing information related to a single point touch event.

    \sa Qul::Platform::SinglePointTouchEventDispatcher
*/

/*!
    \variable Qul::Platform::SinglePointTouchEvent::timestamp

    \since  \QULPlatform 1.4

    \brief The time at which the touch event occured.
*/

/*!
    \variable Qul::Platform::SinglePointTouchEvent::x

    \since  \QULPlatform 1.4

    \brief The x position of the touch event, in screen coordinates.
*/

/*!
    \variable Qul::Platform::SinglePointTouchEvent::y

    \since  \QULPlatform 1.4

    \brief The y position of the touch event, in screen coordinates.
*/

/*!
    \variable Qul::Platform::SinglePointTouchEvent::pressed

    \since  \QULPlatform 1.4

    \brief Whether the touch point is pressed or not.

    When there are no longer any active touch points, a touch event with pressed set to false should be sent.
*/

/*!
    \class Qul::Platform::SinglePointTouchEventDispatcher
    \relatedheader singlepointtoucheventdispatcher
    \inheaderfile platform/singlepointtoucheventdispatcher.h
    \since  \QULPlatform 1.4

    \brief A convenience class for handling single point touch from the platform.

    This class simplifies the job of delivering single point touch events from
    the platform implementation to the \QUL library.

    By using \l dispatch with events of the type \l SinglePointTouchEvent, they
    automatically get delivered to the \QUL library.

    \section2 Example

    Create an instance of the dispatcher:

    \code
    #include <platform/singlepointtoucheventdispatcher.h>

    static Qul::Platform::SinglePointTouchEventDispatcher touchEventDispatcher;
    \endcode

    Now, call dispatch to pass single point touch events on to the \QUL library:

    \code
    touchEventDispatcher.dispatch(Qul::Platform::SinglePointTouchEvent { timestamp, x, y, pressed });
    \endcode

    It might be used in combination with \l Qul::EventQueue like this:

    \code
        class SinglePointTouchEventQueue : public EventQueue<SinglePointTouchEvent>
        {
        public:
            virtual void onEvent(const SinglePointTouchEvent &event)
            {
                touchEventDispatcher.dispatch(event);
            }

        private:
            SinglePointTouchEventDispatcher touchEventDispatcher{&screen};
        };
    \endcode
*/

/*!
    Constructs and initializes the single point touch event dispatcher for the given \a screen.
*/
SinglePointTouchEventDispatcher::SinglePointTouchEventDispatcher(PlatformInterface::Screen *screen)
    : m_screen(screen)
    , m_lastX(-1)
    , m_lastY(-1)
    , m_wasPressed(false)
{}

/*!
    Passes the given \a event on to the \QUL library.
*/
void SinglePointTouchEventDispatcher::dispatch(const SinglePointTouchEvent &event)
{
    if (event.pressed) {
        if (!m_wasPressed || event.x != m_lastX || event.y != m_lastY) {
            PlatformInterface::TouchPoint tp(event.x,
                                             event.y,
                                             m_wasPressed ? PlatformInterface::TouchPoint::Moved
                                                          : PlatformInterface::TouchPoint::Pressed);

            PlatformInterface::handleTouchEvent(m_screen, event.timestamp, &tp, 1);
            m_lastX = event.x;
            m_lastY = event.y;
        }
    } else if (m_wasPressed) {
        PlatformInterface::TouchPoint tp(event.x, event.y, PlatformInterface::TouchPoint::Released);
        PlatformInterface::handleTouchEvent(m_screen, event.timestamp, &tp, 1);
    }
    m_wasPressed = event.pressed;
}

} // namespace Platform
} // namespace Qul
