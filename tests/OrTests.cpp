/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Resources/Or.h"
#include "Resources/Resource.h"

TEST(Or, bothFalse) {
  auto resource = orResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("input0Value")->setValue("false");
  resource->getProperty("input1Value")->setValue("false");
  ASSERT_EQ("false", resource->getProperty("value")->getValue());
}

TEST(Or, firstTrueSecondFalse) {
  auto resource = orResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("input0Value")->setValue("true");
  resource->getProperty("input1Value")->setValue("false");
  ASSERT_EQ("true", resource->getProperty("value")->getValue());
}

TEST(Or, firstFalseSecondTrue) {
  auto resource = orResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("input0Value")->setValue("false");
  resource->getProperty("input1Value")->setValue("true");
  ASSERT_EQ("true", resource->getProperty("value")->getValue());
}

TEST(Or, bothTrue) {
  auto resource = orResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("input0Value")->setValue("true");
  resource->getProperty("input1Value")->setValue("true");
  ASSERT_EQ("true", resource->getProperty("value")->getValue());
}

TEST(Or, threeInputs) {
  auto resource = orResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("inputCount")->setValue("3");
  resource->getProperty("input0Value")->setValue("false");
  resource->getProperty("input1Value")->setValue("false");
  ASSERT_EQ("false", resource->getProperty("value")->getValue());
  resource->getProperty("input2Value")->setValue("true");
  ASSERT_EQ("true", resource->getProperty("value")->getValue());
}