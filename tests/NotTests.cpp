/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gtest/gtest.h"

#include "Resources/Not.h"
#include "Resources/Resource.h"

TEST(Not, true) {
  auto resource = notResourceFactory(std::map<std::string, std::string>{});
  resource->setProperty("inputValue", "true");
  ASSERT_EQ("false", resource->readProperty("value"));
}

TEST(Not, false) {
  auto resource = notResourceFactory(std::map<std::string, std::string>{});
  resource->setProperty("inputValue", "false");
  ASSERT_EQ("true", resource->readProperty("value"));
}
