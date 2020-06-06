#include "Device.hpp"

Device::Device() : m_id(""), m_name("") {
}

Device::Device(const std::string& id, const std::string& name) : m_id(id), m_name(name) {
}

std::string Device::getID() const {
    return m_id;
}

std::string Device::getName() const {
    return m_name;
}
