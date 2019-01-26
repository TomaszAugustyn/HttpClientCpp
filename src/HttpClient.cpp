#include "HttpClient.hpp"

HttpClient::HttpClient(const std::string &domainNameOrIP, unsigned short int port, 
                       const std::string &username, const std::string &password)
                    :   m_domainNameOrIP(domainNameOrIP),
                        m_port(port), 
                        m_username(username),
                        m_password(password)
{
    
}


