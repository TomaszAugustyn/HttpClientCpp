#include "Device.hpp"

Device::Device() : m_id(0), m_name("") {
    
}

Device::Device(int id, const std::string& name) : m_id(id), m_name(name) {
    
}
