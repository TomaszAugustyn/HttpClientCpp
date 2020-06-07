#include "HttpClient.hpp"
#include "TemperatureSensor.hpp"
#include "gtest/gtest.h"
#include <fstream>
#include <limits.h>
#include <memory>

class HttpClientTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        // suppress std::cout when testing
        storedStreambuf = std::cout.rdbuf();
        std::cout.rdbuf(nullptr);
    }

    virtual void TearDown() { std::cout.rdbuf(storedStreambuf); }

    void addDevices(const std::string& deviceType) {
        if (httpClientPtr.get() != NULL) {
            httpClientPtr->addDevices(deviceType);
        } else {
            FAIL() << "httpClientPtr null pointer exception! Cannot proceed.";
        }
    }

    void handleRefreshState(const std::string& deviceType) {
        if (httpClientPtr.get() != NULL) {
            httpClientPtr->handleRefreshState(deviceType);
        } else {
            FAIL() << "httpClientPtr null pointer exception! Cannot proceed.";
        }
    }

    void
    pushBackToDevices(const std::string& id, const std::string& name, const std::string& value) {
        if (httpClientPtr.get() != NULL) {
            auto dev = std::make_shared<TemperatureSensor>(id, name, value);
            httpClientPtr->m_devices.push_back(std::move(dev));
        } else {
            FAIL() << "httpClientPtr null pointer exception! Cannot proceed.";
        }
    }

    std::string getRefreshStateLast() {
        if (httpClientPtr.get() != NULL) {
            return httpClientPtr->m_refreshStateLast;
        }
        return "";
    }

    void setBuffer(const std::string& bufferToSet) {
        if (httpClientPtr.get() != NULL) {
            httpClientPtr->m_buffer = std::string(bufferToSet);
        } else {
            FAIL() << "httpClientPtr null pointer exception! Cannot proceed.";
        }
    }

    std::unique_ptr<HttpClient> httpClientPtr;

private:
    std::streambuf* storedStreambuf;
};

TEST_F(HttpClientTest, queryAPIGetDevices_noThrow) {

    httpClientPtr = std::make_unique<HttpClient>("styx.fibaro.com", "9999", "admin", "admin");
    httpClientPtr->setRunningUnitTest(true);
    httpClientPtr->setTimeout(30L);
    ASSERT_NO_THROW(httpClientPtr->queryAPI(
            TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES));
}

TEST_F(HttpClientTest, queryAPIGetDevices_Throw) {

    httpClientPtr =
            std::make_unique<HttpClient>("amazon.com", "5566", "franeklowcabramek", "asdzxc");
    httpClientPtr->setRunningUnitTest(true);
    httpClientPtr->setTimeout(5L);
    EXPECT_THROW(
            httpClientPtr->queryAPI(
                    TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES),
            std::runtime_error);
}

TEST_F(HttpClientTest, queryAPIRefreshStates_noThrow) {

    httpClientPtr = std::make_unique<HttpClient>("styx.fibaro.com", "9999", "admin", "admin");
    httpClientPtr->setRunningUnitTest(true);
    httpClientPtr->setTimeout(30L);
    httpClientPtr->queryAPI(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::GET_DEVICES);
    ASSERT_NO_THROW(httpClientPtr->queryAPI(
            TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::REFRESH_STATE));
    ASSERT_NO_THROW(httpClientPtr->queryAPI(
            TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::REFRESH_STATE));
}

TEST_F(HttpClientTest, queryAPIRefreshStates_Throw) {

    httpClientPtr = std::make_unique<HttpClient>(
            "spacex.com", "2000", "elonmusk", "morestarshipsplease111");
    httpClientPtr->setRunningUnitTest(true);
    httpClientPtr->setTimeout(5L);
    EXPECT_THROW(
            httpClientPtr->queryAPI(
                    TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR, HttpClient::REFRESH_STATE),
            std::runtime_error);
}

TEST_F(HttpClientTest, addDevices_noThrow) {

    httpClientPtr = std::make_unique<HttpClient>("whatever.com", "2000", "user", "password");
    httpClientPtr->setRunningUnitTest(true);

    std::ifstream ifs("mocked_jsons/api-devices.json"); // load mocked .json
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    setBuffer(content);
    ASSERT_NO_THROW(addDevices(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR));

    auto devices = httpClientPtr->getGetvices();
    ASSERT_EQ(devices.size(), 4);

    for (const auto& device : devices) {
        auto temp = std::dynamic_pointer_cast<TemperatureSensor>(device);
        ASSERT_TRUE(temp);
        std::string id = temp->getID();
        std::string name = temp->getName();
        std::string value = temp->getValue();
        if (id == "7") {
            ASSERT_EQ(name, "temperatureSensor_2");
            ASSERT_EQ(value, "23.17");
        } else if (id == "23") {
            ASSERT_EQ(name, "temperatureSensor_18");
            ASSERT_EQ(value, "0.00");
        } else if (id == "24") {
            ASSERT_EQ(name, "temperatureSensor_19");
            ASSERT_EQ(value, "26.29");
        } else if (id == "25") {
            ASSERT_EQ(name, "temperatureSensor_20");
            ASSERT_EQ(value, "24.48");
        } else {
            FAIL() << "Device ID not found in m_devices vector!";
        }
    }
}

TEST_F(HttpClientTest, addDevices_ThrowEmptyBuffer) {

    httpClientPtr = std::make_unique<HttpClient>("whatever.com", "2000", "user", "password");
    httpClientPtr->setRunningUnitTest(true);

    std::string content = "";
    setBuffer(content);
    EXPECT_THROW(
            {
                try {
                    addDevices(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR);
                } catch (const std::runtime_error& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ(
                            "Buffer is empty! Probably got empty response from "
                            "curl.",
                            e.what());
                    throw;
                }
            },
            std::runtime_error);

    auto devices = httpClientPtr->getGetvices();
    ASSERT_EQ(devices.size(), 0);
}

TEST_F(HttpClientTest, addDevices_ThrowBrokenJson) {

    httpClientPtr = std::make_unique<HttpClient>("whatever.com", "2000", "user", "password");
    httpClientPtr->setRunningUnitTest(true);

    std::ifstream ifs("mocked_jsons/api-devices-broken.json"); // load mocked .json
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    setBuffer(content);
    EXPECT_THROW(
            {
                try {
                    addDevices(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR);
                } catch (const std::runtime_error& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    const std::string errMsg = "Failed to parse JSON! Dumping string: " + content;
                    ASSERT_EQ(errMsg, e.what());
                    throw;
                }
            },
            std::runtime_error);

    auto devices = httpClientPtr->getGetvices();
    ASSERT_EQ(devices.size(), 0);
}

TEST_F(HttpClientTest, handleRefreshState_noThrow) {

    httpClientPtr = std::make_unique<HttpClient>("whatever.com", "2000", "user", "password");
    httpClientPtr->setRunningUnitTest(true);

    pushBackToDevices("7", "temperatureSensor_2", "23.17");
    pushBackToDevices("23", "temperatureSensor_18", "0.00");
    pushBackToDevices("24", "temperatureSensor_19", "26.29");
    pushBackToDevices("25", "temperatureSensor_20", "24.48");

    std::ifstream ifs("mocked_jsons/refreshState-no-param.json"); // load mocked .json
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    setBuffer(content);
    ASSERT_NO_THROW(handleRefreshState(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR));
    auto devices = httpClientPtr->getGetvices();
    ASSERT_EQ(devices.size(), 4);
    ASSERT_EQ(getRefreshStateLast(), "28702");

    std::ifstream ifs2("mocked_jsons/refreshState-with-param.json"); // load mocked .json
    content.assign((std::istreambuf_iterator<char>(ifs2)), (std::istreambuf_iterator<char>()));

    setBuffer(content);
    ASSERT_NO_THROW(handleRefreshState(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR));
    devices = httpClientPtr->getGetvices();
    ASSERT_EQ(devices.size(), 4);
    ASSERT_EQ(getRefreshStateLast(), "28710");

    for (const auto& device : devices) {
        auto temp = std::dynamic_pointer_cast<TemperatureSensor>(device);
        ASSERT_TRUE(temp);
        std::string id = temp->getID();
        std::string name = temp->getName();
        std::string value = temp->getValue();
        if (id == "7") {
            ASSERT_EQ(name, "temperatureSensor_2");
            ASSERT_EQ(value, "19.68");
        } else if (id == "23") {
            ASSERT_EQ(name, "temperatureSensor_18");
            ASSERT_EQ(value, "0.00");
        } else if (id == "24") {
            ASSERT_EQ(name, "temperatureSensor_19");
            ASSERT_EQ(value, "26.29");
        } else if (id == "25") {
            ASSERT_EQ(name, "temperatureSensor_20");
            ASSERT_EQ(value, "19.68");
        } else {
            FAIL() << "Device ID not found in m_devices vector!";
        }
    }
}

TEST_F(HttpClientTest, handleRefreshState_ThrowEmptyBuffer) {

    httpClientPtr = std::make_unique<HttpClient>("whatever.com", "2000", "user", "password");
    httpClientPtr->setRunningUnitTest(true);

    pushBackToDevices("7", "temperatureSensor_2", "23.17");
    pushBackToDevices("23", "temperatureSensor_18", "0.00");
    pushBackToDevices("24", "temperatureSensor_19", "26.29");
    pushBackToDevices("25", "temperatureSensor_20", "24.48");

    std::string content = "";

    setBuffer(content);
    EXPECT_THROW(
            {
                try {
                    handleRefreshState(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR);
                } catch (const std::runtime_error& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ(
                            "Buffer is empty! Probably got empty response "
                            "from curl.",
                            e.what());
                    throw;
                }
            },
            std::runtime_error);

    auto devices = httpClientPtr->getGetvices();
    ASSERT_EQ(devices.size(), 4);
}

TEST_F(HttpClientTest, handleRefreshState_ThrowBrokenJson) {

    httpClientPtr = std::make_unique<HttpClient>("whatever.com", "2000", "user", "password");
    httpClientPtr->setRunningUnitTest(true);

    pushBackToDevices("7", "temperatureSensor_2", "23.17");
    pushBackToDevices("23", "temperatureSensor_18", "0.00");
    pushBackToDevices("24", "temperatureSensor_19", "26.29");
    pushBackToDevices("25", "temperatureSensor_20", "24.48");

    std::ifstream ifs("mocked_jsons/refreshState-broken.json"); // load mocked .json
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    setBuffer(content);
    EXPECT_THROW(
            {
                try {
                    handleRefreshState(TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR);
                } catch (const std::runtime_error& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    const std::string errMsg = "Failed to parse JSON! Dumping string: " + content;
                    ASSERT_EQ(errMsg, e.what());
                    throw;
                }
            },
            std::runtime_error);

    auto devices = httpClientPtr->getGetvices();
    ASSERT_EQ(devices.size(), 4);
}