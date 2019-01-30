#include <limits.h>
#include "gtest/gtest.h"
#include "HttpClient.hpp"
#include <memory>

class HttpClientTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
  
  std::unique_ptr<HttpClient> httpClientPtr;
};

TEST_F(HttpClientTest, test){

    EXPECT_EQ(2, 2);
}

