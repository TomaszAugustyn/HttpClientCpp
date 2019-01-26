/* 
 * File:   IValidator.hpp
 * Author: ubuntu
 *
 * Created on January 25, 2019, 12:16 AM
 */

#ifndef IVALIDATOR_HPP
#define IVALIDATOR_HPP

#include <map>
#include "boost/shared_ptr.hpp"

using ParameterMapPtr = boost::shared_ptr<std::map<std::string, std::string>>;

class IValidator{
    
    public:
        virtual void validate(ParameterMapPtr paramMap) = 0;
        virtual ~IValidator(){}
};



#endif /* IVALIDATOR_HPP */

