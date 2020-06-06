#include "UserInputValidator.hpp"
#include "boost/asio.hpp"
#include "boost/lexical_cast.hpp"
#include <iostream>
#include <stdexcept>

UserInputValidator::UserInputValidator() {
}

void UserInputValidator::validate(ParameterMapPtr paramMap) {

    validateHostnameAndPort(paramMap);
    validateUsername(paramMap);
    validatePassword(paramMap);
}

std::string UserInputValidator::getPort() const {
    return m_port;
}

std::string UserInputValidator::getHostName() const {
    return m_hostName;
}

std::string UserInputValidator::getUsername() const {
    return m_username;
}

std::string UserInputValidator::getPassword() const {
    return m_password;
}

void UserInputValidator::validateHostnameAndPort(ParameterMapPtr paramMap) {

    std::map<std::string, std::string>::iterator it;
    std::string hostName = "";

    it = paramMap->find("port");
    if (it != paramMap->end()) {
        validatePort(it->second);
    } else {
        throw std::invalid_argument("There is no key 'port' in the paramMap!");
    }

    it = paramMap->find("hostname");
    if (it != paramMap->end()) {
        hostName = it->second;
        try {
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::resolver resolver(io_service);
            boost::asio::ip::tcp::resolver::query query(hostName, m_port);
            for (boost::asio::ip::tcp::resolver::iterator i = resolver.resolve(query);
                 i != boost::asio::ip::tcp::resolver::iterator();
                 ++i) {
                boost::asio::ip::tcp::endpoint end = *i;
                std::cout << "Hostname:port has been resolved to: " << end.address() << std::endl;
            }
        } catch (const boost::system::system_error& e) {
            throw std::invalid_argument(
                    "Hostname:port could not be resolved! Exception: " + std::string(e.what()));
        }
    } else {
        throw std::invalid_argument("There is no key 'hostName' in the paramMap!");
    }

    m_hostName = hostName;
}

void UserInputValidator::validatePort(const std::string& port) {

    if (port.empty())
        return;

    try {
        /* do not cast directly to unsigned short int, as passing negative numbers
         * could be wrongly interpreted as positive numbers because of the exceed
         * of variable range, e.g. boost::lexical_cast<unsigned short int>(-1) */
        int portNr = boost::lexical_cast<int>(port);
        if (portNr >= 0 && portNr < 65536)
            m_port = port;
        else
            throw std::invalid_argument("'port' value exceeds port numbers' range [0-65535]!");
    } catch (const boost::bad_lexical_cast& e) {
        throw std::invalid_argument(
                "'port' could not be casted to numeric value! Exception: " + std::string(e.what()));
    }
}

void UserInputValidator::validateUsername(ParameterMapPtr paramMap) {

    std::map<std::string, std::string>::iterator it;

    it = paramMap->find("username");
    if (it != paramMap->end()) {
        if ((it->second).empty())
            throw std::invalid_argument("'username' value is empty!");
    } else {
        throw std::invalid_argument("There is no key 'username' in the paramMap!");
    }

    m_username = it->second;
}

void UserInputValidator::validatePassword(ParameterMapPtr paramMap) {

    std::map<std::string, std::string>::iterator it;

    it = paramMap->find("password");
    if (it != paramMap->end()) {
        if ((it->second).empty())
            throw std::invalid_argument("'password' value is empty!");
    } else {
        throw std::invalid_argument("There is no key 'password' in the paramMap!");
    }

    m_password = it->second;
}