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
        std::string m_port;
        std::string m_hostName;
        std::string m_username;
        std::string m_password;
        
        void validateHostNameAndPort(ParameterMapPtr paramMap);
        void validatePort(const std::string &port);
        void validateUsername(ParameterMapPtr paramMap);
        void validatePassword(ParameterMapPtr paramMap);
    
};



#endif /* INPUTVALIDATOR_HPP */
