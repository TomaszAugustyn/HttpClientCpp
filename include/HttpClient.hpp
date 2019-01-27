/* 
 * File:   HttpClient.hpp
 * Author: ubuntu
 *
 * Created on January 24, 2019, 10:46 PM
 */

#ifndef HTTPCLIENT_HPP
#define HTTPCLIENT_HPP

#include <string>
#include <vector>
#include "Device.hpp"
#include "boost/shared_ptr.hpp"

class HttpClient 
{
    public:
        HttpClient(const std::string &hostName,
                   const std::string &port,
                   const std::string &username,
                   const std::string &password);
        
        std::vector<boost::shared_ptr<Device>> getDevicesFromAPI(const std::string &deviceType);
        
    private:
        const std::string m_hostName;
        const std::string m_port;
        const std::string m_username;
        const std::string m_password;
};



#endif /* HTTPCLIENT_HPP */

