#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <thread>
#include <future>
#include <stdio.h>
#include "curl/curl.h"
#include "HttpClient.hpp"
#include "UserInputValidator.hpp"
#include "TemperatureSensor.hpp"

bool _kbhit()
{
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting > 0;
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
            perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror ("tcsetattr ~ICANON");
    return (buf);
}

void keyPress(boost::shared_ptr<HttpClient> ptr){
    //char key = ' '; 
    while(true){
        if(_kbhit()){
            std::cout<<"keyPress!!" <<std::endl;
            tcflush(0, TCIFLUSH);
            ptr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES);
        }
    }
}

int main()
{
    std::string ip = "", port = "", username = "", password = "";
    boost::shared_ptr<std::map<std::string, std::string> > paramMap(new std::map<std::string, std::string>());
    boost::shared_ptr<HttpClient> httpClientPtr;
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
    
    //httpClientPtr.reset(new HttpClient("styx.fibaro.com", "9999", "admin", "admin"));

    httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES); 
    
    auto f = std::async(std::launch::async, keyPress, httpClientPtr);
    
    while (true){
        httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::REFRESH_STATE);
    }   

    return 0;
}

