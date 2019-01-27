#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include "boost/scoped_ptr.hpp"
#include "curl/curl.h"
#include "HttpClient.hpp"
#include "UserInputValidator.hpp"
#include "TemperatureSensor.hpp"


int main()
{
    //DownloadJSON("styx.fibaro.com:9999/api/devices");
    //DownloadJSON("styx.fibaro.com:9999/api/refreshStates?last=311409");
    std::string ip = "", port = "", username = "", password = "";
    boost::shared_ptr<std::map<std::string, std::string> > paramMap(new std::map<std::string, std::string>());
    boost::scoped_ptr<HttpClient> httpClientPtr;
    UserInputValidator validator;
      
    /*while(true) {
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
                                               validator.getPassword()));*/
    
    httpClientPtr.reset(new HttpClient("styx.fibaro.com", "9999", "admin", "admin"));

    httpClientPtr->getDevicesFromAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR); 
    

    return 0;
}

