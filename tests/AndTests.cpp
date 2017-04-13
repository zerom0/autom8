/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Resources/And.h"
#include "Resources/Resource.h"

TEST(And, bothFalse) {
  auto resource = andResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("input0Value")->setValue("false", true);
  resource->getProperty("input1Value")->setValue("false", true);
  ASSERT_EQ("false", resource->getProperty("value")->getValue());
}

TEST(And, firstTrueSecondFalse) {
  auto resource = andResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("input0Value")->setValue("true", true);
  resource->getProperty("input1Value")->setValue("false", true);
  ASSERT_EQ("false", resource->getProperty("value")->getValue());
}

TEST(And, firstFalseSecondTrue) {
  auto resource = andResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("input0Value")->setValue("false", true);
  resource->getProperty("input1Value")->setValue("true", true);
  ASSERT_EQ("false", resource->getProperty("value")->getValue());
}

TEST(And, bothTrue) {
  auto resource = andResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("input0Value")->setValue("true", true);
  resource->getProperty("input1Value")->setValue("true", true);
  ASSERT_EQ("true", resource->getProperty("value")->getValue());
}

TEST(And, threeInputs) {
  auto resource = andResourceFactory(std::map<std::string, std::string>{});
  resource->getProperty("inputCount")->setValue("3");
  resource->getProperty("input0Value")->setValue("true", true);
  resource->getProperty("input1Value")->setValue("true", true);
  ASSERT_EQ("false", resource->getProperty("value")->getValue());
  resource->getProperty("input2Value")->setValue("true", true);
  ASSERT_EQ("true", resource->getProperty("value")->getValue());
}