/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Resources/And.h"
#include "Resources/Resource.h"

TEST(And, bothFalse) {
  auto resource = andResourceFactory();
  resource->setProperty("input0Value", "false");
  resource->setProperty("input1Value", "false");
  ASSERT_EQ("false", resource->readProperty("value"));
}

TEST(And, firstTrueSecondFalse) {
  auto resource = andResourceFactory();
  resource->setProperty("input0Value", "true");
  resource->setProperty("input1Value", "false");
  ASSERT_EQ("false", resource->readProperty("value"));
}

TEST(And, firstFalseSecondTrue) {
  auto resource = andResourceFactory();
  resource->setProperty("input0Value", "false");
  resource->setProperty("input1Value", "true");
  ASSERT_EQ("false", resource->readProperty("value"));
}

TEST(And, bothTrue) {
  auto resource = andResourceFactory();
  resource->setProperty("input0Value", "true");
  resource->setProperty("input1Value", "true");
  ASSERT_EQ("true", resource->readProperty("value"));
}

TEST(And, threeInputs) {
  auto resource = andResourceFactory();
  resource->setProperty("inputCount", "3");
  resource->setProperty("input0Value", "true");
  resource->setProperty("input1Value", "true");
  ASSERT_EQ("false", resource->readProperty("value"));
  resource->setProperty("input2Value", "true");
  ASSERT_EQ("true", resource->readProperty("value"));
}