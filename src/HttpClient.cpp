#include "HttpClient.hpp"
#include "TemperatureSensor.hpp"
#include "boost/lexical_cast.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>

HttpClient::HttpClient(
        const std::string& hostName,
        const std::string& port,
        const std::string& username,
        const std::string& password)
    : m_hostName(hostName),
      m_port(port),
      m_username(username),
      m_password(password),
      m_timeout(0L),
      m_runningUnitTest(false),
      m_refreshStateLast(""),
      m_buffer("") {
}

std::vector<std::shared_ptr<Device>> HttpClient::getGetvices() {
    return m_devices;
}

void HttpClient::printDevices() const {

    if (!m_runningUnitTest) {
        system("clear"); // system("clear") during running unit tests will make the results poorly
                         // readable (scattered in console)
    }
    std::cout << std::endl;
    for (auto& device : m_devices) {
        std::cout << "deviceID: " << device->getID() << " deviceName: " << device->getName();
        if (auto temp = std::dynamic_pointer_cast<TemperatureSensor>(device)) {
            std::cout << " current value: " << temp->getValue() << std::endl;
        } else {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void HttpClient::setTimeout(unsigned long timeout) {
    m_timeout = timeout;
}

void HttpClient::setRunningUnitTest(bool runningUnitTest) {
    // necessary as gtest doesn't provide any flag/macro to differentiate if actual program is being
    // run or unit tests
    m_runningUnitTest = runningUnitTest;
}

size_t HttpClient::writerCallback(char* data, size_t size, size_t nmemb, void* p) {
    return static_cast<HttpClient*>(p)->writerCallback_impl(data, size, nmemb);
}

size_t HttpClient::writerCallback_impl(char* data, size_t size, size_t nmemb) {
    std::scoped_lock<std::mutex> lock(m_mutex);
    m_buffer.append(data, size * nmemb);
    return size * nmemb;
}

void HttpClient::queryAPI(const std::string& deviceType, CallType callType) {
    CURL* curl;
    CURLcode res;
    char errbuf[CURL_ERROR_SIZE];
    struct curl_slist* headers = NULL;
    std::string jsonStr = "";
    std::string userPwd = std::string(m_username).append(":").append(m_password);
    std::string URL = m_hostName;

    if (!m_port.empty()) {
        URL.append(":").append(m_port);
    }
    if (callType == GET_DEVICES) {
        URL.append("/api/devices");
        std::cout << "GET_DEVICES  "
                  << "m_refreshStateLast = " << m_refreshStateLast << std::endl;
    } else if (callType == REFRESH_STATE) {
        URL.append("/api/refreshStates");
        if (!m_refreshStateLast.empty()) {
            URL.append("?last=").append(m_refreshStateLast);
        }
        std::cout << "REFRESH_STATE  "
                  << "m_refreshStateLast = " << m_refreshStateLast << std::endl;
    }

    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
        curl_easy_setopt(curl, CURLOPT_USERPWD, userPwd.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, m_timeout); // timeout in seconds
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // uncomment to enable verbosity
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->writerCallback);

        errbuf[0] = 0;
        res = curl_easy_perform(curl);
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            if (CURLE_OK == res) {
                if (callType == GET_DEVICES) {
                    addDevices(deviceType);
                } else if (callType == REFRESH_STATE) {
                    handleRefreshState(deviceType);
                }
                m_buffer.clear();
            } else {
                handleCurlError(errbuf, res);
            }
        }
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}

void HttpClient::handleCurlError(char* errbuf, CURLcode& res) {

    size_t len = strlen(errbuf);
    std::string errMsg = "\nlibcurl: ";
    errMsg.append("(").append(boost::lexical_cast<std::string>(res)).append(") ");

    if (len) {
        (errbuf[len - 1] != '\n') ? errMsg.append(errbuf).append("\n") : errMsg.append(errbuf);
    } else {
        errMsg.append(curl_easy_strerror(res)).append("\n");
    }
    m_buffer.clear();
    throw std::runtime_error(errMsg);
}

void HttpClient::addDevices(const std::string& deviceType) {

    Json::Value root;
    Json::Reader reader;
    if (m_buffer.empty()) {
        throw std::runtime_error("Buffer is empty! Probably got empty response from curl.");
    }
    bool parsingSuccessful = reader.parse(m_buffer, root);
    if (!parsingSuccessful) {
        std::string dump(m_buffer);
        m_buffer.clear();
        throw std::runtime_error(
                "Failed to parse JSON! Dumping string: " + std::string(std::move(dump)));
    }
    if (deviceType == TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR) {
        addTemperatureSensors(root);
    }
}

void HttpClient::addTemperatureSensors(const Json::Value& root) {

    for (Json::Value::const_iterator iter = root.begin(); iter != root.end(); iter++) {
        if ((*iter)["type"] == TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR) {
            std::string deviceID = (*iter)["id"].asString();
            std::string deviceName = (*iter)["name"].asString();
            std::string currentValue = (*iter)["properties"]["value"].asString();
            bool createNewTempSensor = true;
            // check if they are already in m_devices vector
            for (const auto& device : m_devices) {
                if ((device->getID() == deviceID) && (device->getName() == deviceName)) {
                    createNewTempSensor = false;
                    if (auto temp = std::dynamic_pointer_cast<TemperatureSensor>(device)) {
                        temp->setValue(currentValue); // update temperature value
                    }
                    break;
                }
            }
            if (createNewTempSensor) {
                auto dev = std::make_shared<TemperatureSensor>(deviceID, deviceName, currentValue);
                m_devices.push_back(std::move(dev));
            }
        }
    }
    printDevices();
}

void HttpClient::handleRefreshState(const std::string& deviceType) {

    Json::Value root;
    Json::Reader reader;
    if (m_buffer.empty()) {
        throw std::runtime_error("Buffer is empty! Probably got empty response from curl.");
    }
    bool parsingSuccessful = reader.parse(m_buffer, root);
    if (!parsingSuccessful) {
        std::string dump(m_buffer);
        m_buffer.clear();
        throw std::runtime_error(
                "Failed to parse JSON! Dumping string: " + std::string(std::move(dump)));
    }
    if (deviceType == TemperatureSensor::DEVICE_TYPE_TEMP_SENSOR) {
        refreshTemperatureSensors(root);
    }
}

void HttpClient::refreshTemperatureSensors(const Json::Value& root) {

    if (m_refreshStateLast.empty()) {
        m_refreshStateLast = root["last"].asString();
        return;
    }
    bool valueChanged = false;
    m_refreshStateLast = root["last"].asString();
    Json::Value changesArray = root["changes"];
    for (Json::Value::const_iterator iter = changesArray.begin(); iter != changesArray.end();
         iter++) {
        std::string deviceID = (*iter)["id"].asString();
        for (auto& device : m_devices) {
            if (device->getID() == deviceID) {
                if (auto temp = std::dynamic_pointer_cast<TemperatureSensor>(device)) {
                    std::string newValue = (*iter)["value"].asString();
                    temp->setValue(newValue); // set new value
                    valueChanged = true;
                }
            }
        }
    }
    if (valueChanged)
        printDevices();
}
