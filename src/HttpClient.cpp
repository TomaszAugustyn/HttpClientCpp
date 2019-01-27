#include "HttpClient.hpp"
#include "curl/curl.h"
#include <iostream>


HttpClient::HttpClient(const std::string &hostName, const std::string &port, 
                       const std::string &username, const std::string &password)
                    :   m_hostName(hostName),
                        m_port(port), 
                        m_username(username),
                        m_password(password)
{
    
}

std::vector<boost::shared_ptr<Device>> HttpClient::getDevicesFromAPI(const std::string &deviceType){
    
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    std::string userPwd = std::string(m_username).append(":").append(m_password);
    std::string URL = m_hostName;
    if(!m_port.empty()){
        URL.append(":").append(m_port);
    }
    URL.append("/api/devices");
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
        res = curl_easy_perform(curl);

        if (CURLE_OK == res) 
        { 
            char *ct;         
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if((CURLE_OK == res) && ct)
            {
                std::cout << ct << std::endl;
            }
        }
        else{
            std::cout<< "ERROR";
            
        }
    }

    curl_slist_free_all(headers); 
    
    std::vector<boost::shared_ptr<Device>> dev;
    return dev;
    
}


