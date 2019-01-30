#include <limits.h>
#include "gtest/gtest.h"
#include "HttpClient.hpp"
#include "TemperatureSensor.hpp"
#include <memory>

class HttpClientTest : public ::testing::Test {
    
    protected:
        virtual void SetUp() {
            //suppress std::cout when testing 
            storedStreambuf = std::cout.rdbuf(); 
            std::cout.rdbuf(nullptr);
        }

        virtual void TearDown() {
            std::cout.rdbuf(storedStreambuf);
        }
  
        std::unique_ptr<HttpClient> httpClientPtr;
  
    private:  
        std::streambuf* storedStreambuf; 
  
};

TEST_F(HttpClientTest, queryAPIGetDevices_noThrow){
    
    httpClientPtr = std::make_unique<HttpClient>("styx.fibaro.com", "9999", "admin", "admin");
    ASSERT_NO_THROW( httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES) );

}

TEST_F(HttpClientTest, queryAPIGetDevices_Throw){
    
    httpClientPtr = std::make_unique<HttpClient>("amazon.com", "5566", "franeklowcabramek", "asdzxc");
    httpClientPtr->setTimeout(5L);
    EXPECT_THROW({
        try{
            httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES);
        }
        catch( const std::invalid_argument &e ){
            // and this tests that it has the correct message (no suitable macro in gtest)
            EXPECT_STREQ( "\nlibcurl: (28) Connection timed out after 5000 milliseconds\n", e.what() );
            throw;
        }
    }, std::runtime_error);
}

TEST_F(HttpClientTest, queryAPIRefreshStates_noThrow){
    
    httpClientPtr = std::make_unique<HttpClient>("styx.fibaro.com", "9999", "admin", "admin");
    httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES);
    ASSERT_NO_THROW( httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::REFRESH_STATE) );
    ASSERT_NO_THROW( httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::REFRESH_STATE) );

}

TEST_F(HttpClientTest, queryAPIRefreshStates_Throw){
    
    httpClientPtr = std::make_unique<HttpClient>("spacex.com", "2000", "elonmusk", "morestarshipsplease111");
    httpClientPtr->setTimeout(5L);
    EXPECT_THROW({
        try{
            httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::REFRESH_STATE);
        }
        catch( const std::invalid_argument &e ){
            // and this tests that it has the correct message (no suitable macro in gtest)
            EXPECT_STREQ( "\nlibcurl: (28) Connection timed out after 5000 milliseconds\n", e.what() );
            throw;
        }
    }, std::runtime_error);
}
