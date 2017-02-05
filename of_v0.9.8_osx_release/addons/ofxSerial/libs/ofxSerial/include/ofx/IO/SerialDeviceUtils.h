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


#include <vector>
#include "ofLog.h"
#include "ofx/IO/AbstractTypes.h"
#include "ofx/IO/DirectoryUtils.h"


namespace ofx {
namespace IO {


class SerialDeviceInfo
{
public:
    typedef std::vector<SerialDeviceInfo> DeviceList;

    SerialDeviceInfo(const std::string& port,
                     const std::string& description,
                     const std::string& hardwareId):
        _port(port),
        _description(description),
        _hardwareId(hardwareId)
    {
    }


    /// \brief Destroy the SerialDeviceInfo.
    virtual ~SerialDeviceInfo()
    {
    }


    /// \brief Get the address of the serial port.
    ///
    /// This can be passed to the constructor of ofxSerial.
    ///
    /// \returns the address of the serial port.
    const std::string& getPort() const
    {
        return _port;
    }


    const std::string& getDescription() const
    {
        return _description;
    }


    const std::string& getHardwareId() const
    {
        return _hardwareId;
    }

    friend std::ostream& operator << (std::ostream& os,
                                      const SerialDeviceInfo& deviceInfo);

protected:
    /// \brief The device's port name.
    std::string _port;

    /// \brief The device's description.
    std::string _description;

    /// \brief The device's hardware id.
    std::string _hardwareId;

};


inline std::ostream& operator << (std::ostream& os,
                                  const SerialDeviceInfo& deviceInfo)
{
    os << deviceInfo._port << ", " << deviceInfo._description << ", " << deviceInfo._hardwareId;
    return os;
}


class SerialDeviceUtils
{
public:
    static SerialDeviceInfo::DeviceList listDevices(const std::string& regexPattern = "",
                                                    int regexOptions = 0,
                                                    bool regexStudy = true);

    static bool sortDevices(const SerialDeviceInfo& device0,
                            const SerialDeviceInfo& device1);

};


} } // namespace ofx::IO
