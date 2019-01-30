#include "HttpClient.hpp"
#include "boost/lexical_cast.hpp"
#include "TemperatureSensor.hpp"
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <stdexcept>


HttpClient::HttpClient(const std::string &hostName, const std::string &port, 
                       const std::string &username, const std::string &password)
                    :   m_hostName(hostName),
                        m_port(port), 
                        m_username(username),
                        m_password(password),
                        m_timeout(30L),
                        m_refreshStateLast(""),
                        m_buffer4GetDevices(""),
                        m_buffer4RefreshStates("")
                        
{
    
}

void HttpClient::printDevices() const{
    system("clear");
    std::cout << std::endl;
    for (auto &device : m_devices){
        std::cout << "deviceID: " << device->getID() << " deviceName: " << device->getName();
        if(boost::shared_ptr<TemperatureSensor> temp = boost::dynamic_pointer_cast<TemperatureSensor>(device)) {
            std::cout << " current value: " << temp->getValue() << std::endl;
        }
        else{
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void HttpClient::setTimeout(unsigned long timeout){
    m_timeout = timeout;
}

size_t HttpClient::writerCallback4DevicesQuery(char *data, size_t size, size_t nmemb, void *p)
{
    return static_cast<HttpClient*>(p)->writerCallback4DevicesQuery_impl(data, size, nmemb);
}

size_t HttpClient::writerCallback4DevicesQuery_impl(char *data, size_t size, size_t nmemb)
{
    m_buffer4GetDevices.append(data, size * nmemb);
    return size * nmemb;
}

size_t HttpClient::writerCallback4RefreshQuery(char *data, size_t size, size_t nmemb, void *p)
{
    return static_cast<HttpClient*>(p)->writerCallback4RefreshQuery_impl(data, size, nmemb);
}

size_t HttpClient::writerCallback4RefreshQuery_impl(char *data, size_t size, size_t nmemb)
{
    m_buffer4RefreshStates.append(data, size * nmemb);
    return size * nmemb;
}

void HttpClient::queryAPI(const std::string &deviceType, CallType callType){
    
    CURL *curl;
    CURLcode res; 
    char errbuf[CURL_ERROR_SIZE];
    struct curl_slist *headers = NULL;
    std::string jsonStr = "";
    std::string userPwd = std::string(m_username).append(":").append(m_password);
    std::string URL = m_hostName;
    if(!m_port.empty()){
        URL.append(":").append(m_port);
    }
    if(callType == GET_DEVICES){
        URL.append("/api/devices");  
        std::cout<< "GET_DEVICES  " << "m_refreshStateLast = " << m_refreshStateLast <<std::endl;
    }
    else if(callType == REFRESH_STATE){
        URL.append("/api/refreshStates");
        if(!m_refreshStateLast.empty()){
            URL.append("?last=").append(m_refreshStateLast);
        }
        std::cout<< "REFRESH_STATE  " << "m_refreshStateLast = " << m_refreshStateLast <<std::endl;
    }
    
    headers = curl_slist_append(headers, "Accept: application/json");  
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8"); 
    curl = curl_easy_init();

    if (curl) 
    {
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1); 
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, userPwd.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_timeout); //timeout in seconds
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); //uncomment to enable verbosity
        if(callType == GET_DEVICES){
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->writerCallback4DevicesQuery);
        }
        else if(callType == REFRESH_STATE){
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->writerCallback4RefreshQuery);
        } 
        errbuf[0] = 0;
        res = curl_easy_perform(curl);

        if (CURLE_OK == res) 
        { 
            if(callType == GET_DEVICES){
                addDevices(deviceType);
            }
            else if(callType == REFRESH_STATE){
                handleRefreshState(deviceType);
            }
        }
        else{
            handleCurlError(errbuf, res);
        }
    }

    curl_slist_free_all(headers); 
    curl_easy_cleanup(curl); 
    if(callType == GET_DEVICES){
        m_buffer4GetDevices.clear();
    }
    else if(callType == REFRESH_STATE){
        m_buffer4RefreshStates.clear();
    } 
}

void HttpClient::handleCurlError(char *errbuf, CURLcode &res){
    
    size_t len = strlen(errbuf);
    std::string errMsg = "\nlibcurl: ";
    errMsg.append("(").append(boost::lexical_cast<std::string>(res)).append(") ");
    
    if(len){
        (errbuf[len - 1] != '\n') ? errMsg.append(errbuf).append("\n") : errMsg.append(errbuf);
    }
    else{
       errMsg.append(curl_easy_strerror(res)).append("\n");
    }       
    
    throw std::runtime_error(errMsg);
}

void HttpClient::addDevices(const std::string &deviceType){
    
    Json::Value root;
    Json::Reader reader;
    if(m_buffer4GetDevices.empty()){
        throw std::runtime_error("Buffer m_buffer4GetDevices is empty! Probably got empty response from curl.");
    }
    bool parsingSuccessful = reader.parse(m_buffer4GetDevices, root);
    if (!parsingSuccessful)
    {
        throw std::runtime_error("Failed to parse JSON! Dumping string: " + std::string(m_buffer4GetDevices));
    }
    if(deviceType == TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR){
        addTemperatureSensors(root);
    }
       
}

void HttpClient::addTemperatureSensors(const Json::Value &root){
    
    for( Json::Value::const_iterator iter = root.begin() ; iter != root.end() ; iter++ )
    {
        if ((*iter)["type"] == TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR){
            std::string deviceID = (*iter)["id"].asString();
            std::string deviceName = (*iter)["name"].asString();
            std::string currentValue = (*iter)["properties"]["value"].asString();
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
    printDevices();
}

void HttpClient::handleRefreshState(const std::string &deviceType){
    
    Json::Value root;
    Json::Reader reader;
    if(m_buffer4RefreshStates.empty()){
        throw std::runtime_error("Buffer m_buffer4RefreshStates is empty! Probably got empty response from curl.");
    }
    bool parsingSuccessful = reader.parse(m_buffer4RefreshStates, root);
    if (!parsingSuccessful)
    {
        throw std::runtime_error("Failed to parse JSON! Dumping string: " + std::string(m_buffer4RefreshStates));
    }   
    if(deviceType == TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR){
        refreshTemperatureSensors(root);
    }
    
}

void HttpClient::refreshTemperatureSensors(const Json::Value &root){
   
    if(m_refreshStateLast.empty()){
        m_refreshStateLast = root["last"].asString();
        return;
    }
    bool valueChanged = false;
    m_refreshStateLast = root["last"].asString();
    Json::Value changesArray = root["changes"];
    for( Json::Value::const_iterator iter = changesArray.begin() ; iter != changesArray.end() ; iter++ )
    {
        std::string deviceID = (*iter)["id"].asString();
        for (auto &device : m_devices){
            if(device->getID() == deviceID){
                if(boost::shared_ptr<TemperatureSensor> temp = boost::dynamic_pointer_cast<TemperatureSensor>(device)) {
                    std::string newValue = (*iter)["value"].asString();
                    temp->setValue(newValue); //set new value
                    valueChanged = true;
                }
            }  
        }
    }
    if(valueChanged)
        printDevices();
}

