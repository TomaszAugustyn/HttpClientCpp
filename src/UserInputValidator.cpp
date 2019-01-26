#include "UserInputValidator.hpp"
#include <stdexcept>
#include "boost/lexical_cast.hpp"


void UserInputValidator::validate(ParameterMapPtr paramMap){
    
    std::map<std::string, std::string>::iterator it;    
    
    it = paramMap->find("domainNameOrIP");
    if (it != paramMap->end())
        validateDomainNameOrIP(it->second);
    else{
        throw std::invalid_argument( "There is no key \"domainNameOrIP\" in the paramMap!" );
    }
    
    it = paramMap->find("port");
    if (it != paramMap->end())
        validatePort(it->second);
    else{
        throw std::invalid_argument( "There is no key \"port\" in the paramMap!" );
    }
    
    it = paramMap->find("username");
    if (it != paramMap->end())
        validateUsername(it->second);
    else{
        throw std::invalid_argument( "There is no key \"username\" in the paramMap!" );
    }
    
    it = paramMap->find("password");
    if (it != paramMap->end())
        validatePassword(it->second);
    else{
        throw std::invalid_argument( "There is no key \"password\" in the paramMap!" );
    }
    
}


void UserInputValidator::validateDomainNameOrIP(const std::string &domainNameOrIP){
    
    
    
    m_domainNameOrIP = domainNameOrIP;
}

void UserInputValidator::validatePort(const std::string &port){
    
    try{
        /* do not cast directly to unsigned short int, as passing negative numbers
         * could be wrongly interpreted as positive numbers because of the exceed 
         * of variable range, e.g. boost::lexical_cast<unsigned short int>(-1) */
        int portNr = boost::lexical_cast<int>(port);
        if(portNr >= 0 && portNr < 65536)
            m_port = boost::lexical_cast<unsigned short int>(portNr);
        else
            throw std::invalid_argument( "\"port\" value exceeds port numbers' range [0-65535]!" );
            
    }
    catch (const boost::bad_lexical_cast& e){      
        throw std::invalid_argument( "\"port\" could not be casted to numeric value! Exception: " + std::string(e.what()) ); 
    }
    
}


void UserInputValidator::validateUsername(const std::string &username){
    
    if(username.empty())
       throw std::invalid_argument( "\"username\" value is empty!" ); 
    
    m_username = username;
}


void UserInputValidator::validatePassword(const std::string &password){
    
    if(password.empty())
       throw std::invalid_argument( "\"password\" value is empty!" ); 

    m_password = password;
}