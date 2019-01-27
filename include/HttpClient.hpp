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
#include <cstddef> 
#include "Device.hpp"
#include "boost/shared_ptr.hpp"
#include "jsoncpp/json/json.h"

class HttpClient 
{
    public:
        HttpClient(const std::string &hostName,
                   const std::string &port,
                   const std::string &username,
                   const std::string &password);
        
        void getDevicesFromAPI(const std::string &deviceType);
        std::vector<boost::shared_ptr<Device> > getGetvices() const;
        
    private:
        const std::string m_hostName;
        const std::string m_port;
        const std::string m_username;
        const std::string m_password;
        std::vector<boost::shared_ptr<Device> > m_devices;
      
        std::string m_curlBuffer;
        static size_t curlWriterCallbackFunc(char *data, size_t size, size_t nmemb, void *p);
        size_t curlWriterCallbackFunc_impl(char *data, size_t size, size_t nmemb);
        
        void addDevices(const std::string &deviceType);
        void addTemperatureSensors(const Json::Value &root);
        
};



#endif /* HTTPCLIENT_HPP */

