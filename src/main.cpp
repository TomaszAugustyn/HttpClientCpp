#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include "boost/scoped_ptr.hpp"
#include "curl/curl.h"
#include "HttpClient.hpp"
#include "UserInputValidator.hpp"
#include "TemperatureSensor.hpp"

/*static std::string *DownloadedResponse;

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer_in)
{

    // Is there anything in the buffer?  
    if (buffer_in != NULL)  
    {
        // Append the data to the buffer    
        buffer_in->append(data, size * nmemb);

        // How much did we write?   
        DownloadedResponse = buffer_in;

        return size * nmemb;  
    }

    return 0;

}   


std::string DownloadJSON(std::string URL)
{   
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers=NULL; // init to NULL is important 
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
        curl_easy_setopt(curl, CURLOPT_USERPWD, "admin:admin");
        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        res = curl_easy_perform(curl);

        if (CURLE_OK == res) 
        { 
            char *ct;         
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if((CURLE_OK == res) && ct)
            {
                std::cout << ct;
                //std::cout<< *DownloadedResponse;  
                //return *DownloadedResponse;
            }

        }
        else{
            std::cout<< "ERROR";
            
        }
    }

    curl_slist_free_all(headers);
    return "";
}*/


int main()
{
    //DownloadJSON("styx.fibaro.com:9999/api/devices");
    //DownloadJSON("styx.fibaro.com:9999/api/refreshStates?last=311409");
    std::string ip = "", port = "", username = "", password = "";
    boost::shared_ptr<std::map<std::string, std::string> > paramMap(new std::map<std::string, std::string>());
    boost::scoped_ptr<HttpClient> httpClientPtr;
    UserInputValidator validator;
      
    while(true) {
        try{
            std::cout << "Enter IP address/hostname of the central: ";
            getline(std::cin, (*paramMap)["hostName"], '\n');
            std::cout << "Enter port number [0-65535]: ";
            getline(std::cin, (*paramMap)["port"], '\n');
            std::cout << "Enter username: ";
            getline(std::cin, (*paramMap)["username"], '\n');
            std::cout << "Enter password: ";
            getline(std::cin, (*paramMap)["password"], '\n');

            validator.validate(paramMap);
            break;
        }
        catch(const std::invalid_argument &e){
            std::cout << "An Exception occured: " << e.what() << std::endl;
            std::cout << "Please try entering parameters again." << std::endl;
        }
    }
    
    httpClientPtr.reset(new HttpClient(validator.getHostName(), 
                                               validator.getPort(),
                                               validator.getUsername(),
                                               validator.getPassword()));

    httpClientPtr->getDevicesFromAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR);  

    return 0;
}

