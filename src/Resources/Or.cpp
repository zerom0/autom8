/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Or.h"

#include "Resource.h"

Resource* newOrResource(ResourceChangedCallback callback) {
  auto r = new Resource(callback);
  r->createProperty("value", Property{false});
  r->createProperty("inputCount", Property{std::bind(inputCountUpdated, r, "inputCount", std::placeholders::_1, std::placeholders::_2)});
  r->updateProperty("inputCount", "2");
  return r;
}

void orResourceUpdated(Resource* resource, const std::string& propertyName) {
  if (propertyName.find("input") != 0) return;

  auto count = 0UL;
  try {
    count = std::stoul(resource->readProperty("inputCount"));
  } catch (std::exception& e) {

  }

  std::string value = "false";
  for (unsigned i = 0; i < count; ++i) {
    if (resource->readProperty("input" + std::to_string(i) + "Value") == "true") {
      value = "true";
      break;
    }
  }

  resource->setProperty("value", value);
}

std::unique_ptr<Resource> orResourceFactory() {
  return std::unique_ptr<Resource>(newOrResource(orResourceUpdated));
}
