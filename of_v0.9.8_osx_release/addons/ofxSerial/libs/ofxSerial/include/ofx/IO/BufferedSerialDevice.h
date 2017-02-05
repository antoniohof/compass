// =============================================================================
//
// Copyright (c) 2010-2016 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#pragma once


#include "ofx/IO/SerialEvents.h"
#include "ofx/IO/SerialDevice.h"


namespace ofx {
namespace IO {


/// \brief A Serial Device that emits events when a buffer has been filled.
class BufferedSerialDevice: public SerialDevice
{
public:
    BufferedSerialDevice(uint8_t marker = DEFAULT_MARKER,
                         std::size_t maxBufferSize = DEFAULT_MAX_BUFFER_SIZE);

    /// \Brief destroy the BufferedSerialDevice.
    virtual ~BufferedSerialDevice();

    void update(ofEventArgs& args);

    /// \brief Set a end of line (EOL) marker.
    /// \param data the EOL marker.
    void setMarker(uint8_t marker);

    /// \brief Get the end of line (EOL) marker.
    /// \returns the EOL marker.
    uint8_t getMarker() const;

    /// \brief Clear buffer.
    void clear();

    std::size_t getMaximumBufferSize() const;

    void setMaximumBufferSize(std::size_t size);

    /// \brief Register a class to receive notifications for all events.
    /// \param listener a pointer to the listener class.
    /// \param order the event order.
    template<class ListenerClass>
    void registerAllEvents(ListenerClass* listener, int order = OF_EVENT_ORDER_AFTER_APP)
    {
        ofAddListener(events.onSerialBuffer, listener, &ListenerClass::onSerialBuffer, order);
        ofAddListener(events.onSerialError, listener, &ListenerClass::onSerialError, order);
    }

    /// \brief Unregister a class to receive notifications for all events.
    /// \param listener a pointer to the listener class.
    template<class ListenerClass>
    void unregisterAllEvents(ListenerClass* listener, int order = OF_EVENT_ORDER_AFTER_APP)
    {
        ofRemoveListener(events.onSerialBuffer, listener, &ListenerClass::onSerialBuffer, order);
        ofRemoveListener(events.onSerialError, listener, &ListenerClass::onSerialError, order);
    }

    /// \brief The SerialEvents that the user can subscribe to.
    SerialEvents events;

    /// \brief The default marker is the new line character (ASCII \n 10)
    static const uint8_t DEFAULT_MARKER;

    enum
    {
        DEFAULT_MAX_BUFFER_SIZE = 8192
    };

protected:

    /// \brief The buffer boundary marker.
    uint8_t _marker;

    /// \brief The byte buffer.
    ByteBuffer _buffer;

    /// \brief The maximum size of the boundary.
    std::size_t _maxBufferSize;

    enum
    {
        UPDATE_BUFFER_SIZE = 2048
    };

};


} } // namespace ofx::IO
