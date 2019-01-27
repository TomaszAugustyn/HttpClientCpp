#include "TemperatureSensor.hpp"

const std::string TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR = "com.fibaro.temperatureSensor";

TemperatureSensor::TemperatureSensor() : Device(), m_value(0.0) {
    
}

TemperatureSensor::TemperatureSensor(int id, const std::string& name, double value) : Device(id, name), m_value(value){
    
}

void TemperatureSensor::setValue(double value){
    
    m_value = value;
}
