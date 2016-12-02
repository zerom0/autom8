/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "And.h"

#include "Resource.h"


#include <coap/json.h>

Resource* newAndResource(ResourceChangedCallback callback, const std::map<std::string, std::string>& values) {
  auto r = new Resource(callback);
  r->createProperty("value", Property{false, false});
  r->createProperty("inputCount", Property{std::bind(inputCountUpdated, r, "inputCount", std::placeholders::_1, std::placeholders::_2), true});
  r->updateProperty("inputCount", getValueOr(values, "inputCount", "2"));

  for (auto it = begin(values); it != end(values); ++it) r->updateProperty(it->first, it->second);

  return r;
}

void andResourceUpdated(Resource* resource, const std::string& propertyName) {
  if (propertyName.find("input") != 0) return;

  try {
    unsigned count;
    CoAP::from_json(resource->readProperty("inputCount"), count);

    auto value = true;

    for (unsigned i = 0; i < count; ++i) {
      bool inputValue;
      CoAP::from_json(resource->readProperty("input" + std::to_string(i) + "Value"), inputValue);
      value &= inputValue;
    }

    resource->setProperty("value", CoAP::to_json(value));
  }
  catch (std::runtime_error& e) {

  }
}

std::unique_ptr<Resource> andResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newAndResource(andResourceUpdated, values));
}
