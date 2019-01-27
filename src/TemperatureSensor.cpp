#include "TemperatureSensor.hpp"

const std::string TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR = "com.fibaro.temperatureSensor";

TemperatureSensor::TemperatureSensor() : Device(), m_value("") {
    
}

TemperatureSensor::TemperatureSensor(const std::string &id, const std::string& name, const std::string &value) : Device(id, name), m_value(value){
    
}

void TemperatureSensor::setValue(const std::string &value){  
    m_value = value;
}

std::string TemperatureSensor::getValue() const{
    return m_value;
}