/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Not.h"

#include "Resource.h"

Resource* newNotResource(ResourceChangedCallback callback) {
  auto r = new Resource(callback);
  r->createProperty("value", Property{false});
  r->createProperty("inputURI", Property{std::bind(inputURIUpdated, r, "inputURI", std::placeholders::_1, std::placeholders::_2)});
  r->createProperty("inputValue", Property{false});
  return r;
}

void notResourceUpdated(Resource* resource, const std::string& propertyName) {
  if (propertyName.find("input") != 0) return;

  std::string value = "true";

  if (resource->readProperty("inputValue") == "true") {
    value = "false";
  }

  resource->updateProperty("value", value);
}

Resource* notResourceFactory() {
  return newNotResource(notResourceUpdated);
}