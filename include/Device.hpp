/* 
 * File:   Device.hpp
 * Author: ubuntu
 *
 * Created on January 27, 2019, 12:03 AM
 */

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>

class Device {
    
    public:
        Device();
        Device(const std::string &id, const std::string &name);
        virtual ~Device() = default;
        std::string getID() const;
        std::string getName() const;     
        
    protected:
        std::string m_id;
        std::string m_name;
        
};


#endif /* DEVICE_HPP */

