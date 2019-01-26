#include "HttpClient.hpp"

HttpClient::HttpClient(const std::string &hostName, const std::string &port, 
                       const std::string &username, const std::string &password)
                    :   m_hostName(hostName),
                        m_port(port), 
                        m_username(username),
                        m_password(password)
{
    
}


