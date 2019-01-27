#include "HttpClient.hpp"
#include "curl/curl.h"
#include "boost/make_shared.hpp"
#include "TemperatureSensor.hpp"
#include <iostream>


HttpClient::HttpClient(const std::string &hostName, const std::string &port, 
                       const std::string &username, const std::string &password)
                    :   m_hostName(hostName),
                        m_port(port), 
                        m_username(username),
                        m_password(password),
                        m_curlBuffer("")
{
    
}

size_t HttpClient::curlWriterCallbackFunc(char *data, size_t size, size_t nmemb, void *p)
{
    return static_cast<HttpClient*>(p)->curlWriterCallbackFunc_impl(data, size, nmemb);
}

size_t HttpClient::curlWriterCallbackFunc_impl(char *data, size_t size, size_t nmemb)
{
    m_curlBuffer.append(data, size * nmemb);
    return size * nmemb;
}

void HttpClient::getDevicesFromAPI(const std::string &deviceType){
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    std::string jsonStr = "";
    std::string userPwd = std::string(m_username).append(":").append(m_password);
    std::string URL = m_hostName;
    if(!m_port.empty()){
        URL.append(":").append(m_port);
    }
    URL.append("/api/devices");
    //URL.append("/api/refreshStates");
    headers = curl_slist_append(headers, "Accept: application/json");  
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8"); 
    curl = curl_easy_init();

    if (curl) 
    {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1); 
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, userPwd.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->curlWriterCallbackFunc);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);

        if (CURLE_OK == res) 
        { 
            char *ct; // memory will br freed automatically by "curl_easy_cleanup"
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if((CURLE_OK == res) && ct)
            {
                std::cout << "Content-Type: " << ct << std::endl << std::endl;  
                addDevices(deviceType);
            }
        }
        else{
            std::cout<< "ERROR";
        }
    }

    curl_slist_free_all(headers); 
    curl_easy_cleanup(curl); 
    m_curlBuffer.clear();
    
}

std::vector<boost::shared_ptr<Device> > HttpClient::getGetvices() const{
    return m_devices;
}

void HttpClient::addDevices(const std::string &deviceType){
    
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(m_curlBuffer, root);
    if (!parsingSuccessful)
    {
        std::cout << "Error parsing the string" << std::endl;
    }
    if(deviceType == TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR){
        addTemperatureSensors(root);
    }
       
}

void HttpClient::addTemperatureSensors(const Json::Value &root){
    
    for( Json::Value::const_iterator outer = root.begin() ; outer != root.end() ; outer++ )
    {
        if ((*outer)["type"] == TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR){
            std::string deviceID = (*outer)["id"].asString();
            std::string deviceName = (*outer)["name"].asString();
            std::string currentValue = (*outer)["properties"]["value"].asString();
            std::cout << "deviceID: " << deviceID << " deviceName: " << deviceName << " current value: " << currentValue <<std::endl;
            bool createNewTempSensor = true;
            // check if they are already in m_devices vector
            for (auto &device : m_devices){
                if((device->getID() == deviceID) && (device->getName() == deviceName)){
                    createNewTempSensor = false;
                    if(boost::shared_ptr<TemperatureSensor> temp = boost::dynamic_pointer_cast<TemperatureSensor>(device)) {
                        temp->setValue(currentValue); //update temperature value
                    }
                    break;
                }
            }
            if(createNewTempSensor){
                boost::shared_ptr<TemperatureSensor> dev( new TemperatureSensor(deviceID, deviceName, currentValue) );
                m_devices.push_back(dev);    
            }
        }              
    } 
    
}