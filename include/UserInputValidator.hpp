/* 
 * File:   InputValidator.hpp
 * Author: ubuntu
 *
 * Created on January 24, 2019, 11:32 PM
 */

#ifndef USERINPUTVALIDATOR_HPP
#define USERINPUTVALIDATOR_HPP

#include "IValidator.hpp"
#include "gtest/gtest_prod.h"
#include <string>

class UserInputValidator : public IValidator {  
    
    public:
        UserInputValidator();
        void validate(ParameterMapPtr paramMap);
        
        std::string getPort() const;
        std::string getHostName() const;
        std::string getUsername() const;
        std::string getPassword() const;
        
    private:
        std::string m_port;
        std::string m_hostName;
        std::string m_username;
        std::string m_password;
        
        void validateHostnameAndPort(ParameterMapPtr paramMap);
        void validatePort(const std::string &port);
        void validateUsername(ParameterMapPtr paramMap);
        void validatePassword(ParameterMapPtr paramMap);
        
        friend class UserInputValidatorTest;
    
};



#endif /* INPUTVALIDATOR_HPP */

