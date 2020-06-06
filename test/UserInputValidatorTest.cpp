#include "UserInputValidator.hpp"
#include "gtest/gtest.h"
#include <limits.h>
#include <stdexcept>

class UserInputValidatorTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        // suppress std::cout when testing
        storedStreambuf = std::cout.rdbuf();
        std::cout.rdbuf(nullptr);
    }

    virtual void TearDown() { std::cout.rdbuf(storedStreambuf); }

    void validatePort(const std::string& port) { validator.validatePort(port); }
    void validateHostnameAndPort(ParameterMapPtr paramMap) {
        validator.validateHostnameAndPort(paramMap);
    }
    void validateUsername(ParameterMapPtr paramMap) { validator.validateUsername(paramMap); }
    void validatePassword(ParameterMapPtr paramMap) { validator.validatePassword(paramMap); }

    UserInputValidator validator;

private:
    std::streambuf* storedStreambuf;
};

TEST_F(UserInputValidatorTest, validate) {

    auto paramMap = std::make_shared<std::map<std::string, std::string>>();
    (*paramMap)["hostname"] = "styx.fibaro.com";
    (*paramMap)["port"] = "9999";
    (*paramMap)["username"] = "admin";
    (*paramMap)["password"] = "admin";

    ASSERT_NO_THROW(validator.validate(paramMap));
}

TEST_F(UserInputValidatorTest, validatePortPositive) {

    ASSERT_NO_THROW(validatePort("0"));
    ASSERT_NO_THROW(validatePort("20"));
    ASSERT_NO_THROW(validatePort("22"));
    ASSERT_NO_THROW(validatePort("53"));
    ASSERT_NO_THROW(validatePort("80"));
    ASSERT_NO_THROW(validatePort("9999"));
    ASSERT_NO_THROW(validatePort("65535"));
}

TEST_F(UserInputValidatorTest, validatePortNegative) {

    EXPECT_THROW(
            {
                try {
                    validatePort("-1");
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ("'port' value exceeds port numbers' range [0-65535]!", e.what());
                    throw;
                }
            },
            std::invalid_argument);

    EXPECT_THROW(
            {
                try {
                    validatePort("65536");
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ("'port' value exceeds port numbers' range [0-65535]!", e.what());
                    throw;
                }
            },
            std::invalid_argument);

    EXPECT_THROW(
            {
                try {
                    validatePort("Nope...not a port");
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ(
                            "'port' could not be casted to numeric value! Exception: "
                            "bad lexical cast: source type value could not be interpreted as "
                            "target",
                            e.what());
                    throw;
                }
            },
            std::invalid_argument);
}

TEST_F(UserInputValidatorTest, validateHostnameAndPortPositive) {

    auto paramMap = std::make_shared<std::map<std::string, std::string>>();

    (*paramMap)["hostname"] = "google.com";
    (*paramMap)["port"] = "2000";
    ASSERT_NO_THROW(validateHostnameAndPort(paramMap));
    (*paramMap)["hostname"] = "stackoverflow.com";
    (*paramMap)["port"] = "80";
    ASSERT_NO_THROW(validateHostnameAndPort(paramMap));
    (*paramMap)["hostname"] = "bigbrother.tvn.pl";
    (*paramMap)["port"] = "";
    ASSERT_NO_THROW(validateHostnameAndPort(paramMap));
}

TEST_F(UserInputValidatorTest, validateHostnameAndPortNegative) {

    auto paramMap = std::make_shared<std::map<std::string, std::string>>();
    (*paramMap)["hostname"] = "wp.pl";

    EXPECT_THROW(
            {
                try {
                    validateHostnameAndPort(paramMap);
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ("There is no key 'port' in the paramMap!", e.what());
                    throw;
                }
            },
            std::invalid_argument);

    paramMap.reset(new std::map<std::string, std::string>());
    (*paramMap)["port"] = "5000";

    EXPECT_THROW(
            {
                try {
                    validateHostnameAndPort(paramMap);
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ("There is no key 'hostName' in the paramMap!", e.what());
                    throw;
                }
            },
            std::invalid_argument);

    (*paramMap)["hostname"] = "ThisDomainWillNeverEverExistForSureAndILikeWarmCarpetSlippers.com";

    EXPECT_THROW(
            {
                try {
                    validateHostnameAndPort(paramMap);
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ(
                            "Hostname:port could not be resolved! Exception:"
                            " resolve: Host not found (authoritative)",
                            e.what());
                    throw;
                }
            },
            std::invalid_argument);
}

TEST_F(UserInputValidatorTest, validateUsernamePositive) {

    auto paramMap = std::make_shared<std::map<std::string, std::string>>();

    (*paramMap)["username"] = "Elliot Alderson";
    ASSERT_NO_THROW(validateUsername(paramMap));
    (*paramMap)["username"] = "Cebulak123";
    ASSERT_NO_THROW(validateUsername(paramMap));
    (*paramMap)["username"] = "Proboscis monkey";
    ASSERT_NO_THROW(validateUsername(paramMap));
}

TEST_F(UserInputValidatorTest, validateUsernameNegative) {

    auto paramMap = std::make_shared<std::map<std::string, std::string>>();

    EXPECT_THROW(
            {
                try {
                    validateUsername(paramMap);
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ("There is no key 'username' in the paramMap!", e.what());
                    throw;
                }
            },
            std::invalid_argument);

    (*paramMap)["username"] = "";

    EXPECT_THROW(
            {
                try {
                    validateUsername(paramMap);
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ("'username' value is empty!", e.what());
                    throw;
                }
            },
            std::invalid_argument);
}

TEST_F(UserInputValidatorTest, validatePasswordPositive) {

    auto paramMap = std::make_shared<std::map<std::string, std::string>>();

    (*paramMap)["password"] = "sweetKittens888";
    ASSERT_NO_THROW(validatePassword(paramMap));
    (*paramMap)["password"] = "qwerty";
    ASSERT_NO_THROW(validatePassword(paramMap));
    (*paramMap)["password"] = "youwillneverguess";
    ASSERT_NO_THROW(validatePassword(paramMap));
}

TEST_F(UserInputValidatorTest, validatePasswordNegative) {

    auto paramMap = std::make_shared<std::map<std::string, std::string>>();

    EXPECT_THROW(
            {
                try {
                    validatePassword(paramMap);
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ("There is no key 'password' in the paramMap!", e.what());
                    throw;
                }
            },
            std::invalid_argument);

    (*paramMap)["password"] = "";

    EXPECT_THROW(
            {
                try {
                    validatePassword(paramMap);
                } catch (const std::invalid_argument& e) {
                    // and this tests that it has the correct message (no suitable macro in gtest)
                    EXPECT_STREQ("'password' value is empty!", e.what());
                    throw;
                }
            },
            std::invalid_argument);
}
