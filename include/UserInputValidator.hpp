/* 
 * File:   InputValidator.hpp
 * Author: ubuntu
 *
 * Created on January 24, 2019, 11:32 PM
 */

#ifndef USERINPUTVALIDATOR_HPP
#define USERINPUTVALIDATOR_HPP

#include "IValidator.hpp"


class UserInputValidator : public IValidator {
    
    public:
        void validate(ParameterMapPtr paramMap);
        
    private:
        unsigned short int m_port;
        std::string m_domainNameOrIP;
        std::string m_username;
        std::string m_password;
        
        void validateDomainNameOrIP(const std::string &domainNameOrIP);
        void validatePort(const std::string &port);
        void validateUsername(const std::string &username);
        void validatePassword(const std::string &password);
    
};



#endif /* INPUTVALIDATOR_HPP */

