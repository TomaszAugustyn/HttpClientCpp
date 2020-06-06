#include "HttpClient.hpp"
#include "TemperatureSensor.hpp"
#include "UserInputValidator.hpp"
#include <future>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

bool _kbhit() {
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
    struct termios old = { 0 };
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}

void keyPress(std::shared_ptr<HttpClient> ptr) {
    // char key = ' ';
    while (true) {
        if (_kbhit()) {
            std::cout << "Key pressed, refreshing..." << std::endl;
            tcflush(0, TCIFLUSH);
            ptr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES);
        }
    }
}

int main() {
    std::string ip = "", port = "", username = "", password = "";
    auto paramMap = std::make_shared<std::map<std::string, std::string>>();
    UserInputValidator validator;

    while (true) {
        try {
            std::cout << "Enter IP address/hostname of the central: ";
            getline(std::cin, (*paramMap)["hostname"], '\n');
            std::cout << "Enter port number [0-65535]: ";
            getline(std::cin, (*paramMap)["port"], '\n');
            std::cout << "Enter username: ";
            getline(std::cin, (*paramMap)["username"], '\n');
            std::cout << "Enter password: ";
            getline(std::cin, (*paramMap)["password"], '\n');

            validator.validate(paramMap);
            break;
        } catch (const std::invalid_argument& e) {
            std::cout << "An Exception occured: " << e.what() << std::endl;
            std::cout << "Please try entering parameters again." << std::endl;
        }
    }

    auto httpClientPtr = std::make_shared<HttpClient>(
            validator.getHostName(),
            validator.getPort(),
            validator.getUsername(),
            validator.getPassword());

    // For testing
    // httpClientPtr.reset(new HttpClient("styx.fibaro.com", "9999", "admin", "admin"));

    try {
        httpClientPtr->queryAPI(
                TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES);
        auto f = std::async(std::launch::async, keyPress, httpClientPtr);
        while (true) {
            httpClientPtr->queryAPI(
                    TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::REFRESH_STATE);
        }
    } catch (const std::runtime_error& e) {
        std::cout << "An Exception occured: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
