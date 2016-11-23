/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Or.h"

#include "Resource.h"

#include <coap/json.h>

Resource* newOrResource(ResourceChangedCallback callback) {
  auto r = new Resource(callback);
  r->createProperty("value", Property{false, false});
  r->createProperty("inputCount", Property{std::bind(inputCountUpdated, r, "inputCount", std::placeholders::_1, std::placeholders::_2), true});
  r->updateProperty("inputCount", "2");
  return r;
}

void orResourceUpdated(Resource* resource, const std::string& propertyName) {
  if (propertyName.find("input") != 0) return;

  try {
    unsigned count;
    CoAP::from_json(resource->readProperty("inputCount"), count);

    auto value = false;

    for (unsigned i = 0; i < count; ++i) {
      bool inputValue;
      CoAP::from_json(resource->readProperty("input" + std::to_string(i) + "Value"), inputValue);
      value |= inputValue;
    }

    resource->setProperty("value", CoAP::to_json(value));
  }
  catch (std::runtime_error& e) {

  }
}

std::unique_ptr<Resource> orResourceFactory() {
  return std::unique_ptr<Resource>(newOrResource(orResourceUpdated));
}
