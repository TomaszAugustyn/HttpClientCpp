/*
 * File:   TemperatureSensor.hpp
 * Author: ubuntu
 *
 * Created on January 27, 2019, 12:54 AM
 */

#ifndef TEMPERATURESENSOR_HPP
#define TEMPERATURESENSOR_HPP

#include "Device.hpp"

class TemperatureSensor : public Device {

public:
    TemperatureSensor();
    TemperatureSensor(const std::string& id, const std::string& name, const std::string& value);
    static const std::string DEVICE_TYPE_TEMP_SENSOR;
    void setValue(const std::string& value);
    std::string getValue() const;

private:
    std::string m_value;
};

#endif /* TEMPERATURESENSOR_HPP */
