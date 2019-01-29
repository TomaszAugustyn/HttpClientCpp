#include <limits.h>
#include "gtest/gtest.h"
#include "HttpClient.hpp"

class HttpClientTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(HttpClientTest, test){

    EXPECT_EQ(2, 2);
}

