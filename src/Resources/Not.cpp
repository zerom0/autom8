/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Not.h"

#include "Resource.h"

#include <coap/json.h>

Resource* newNotResource(ResourceChangedCallback callback) {
  auto r = new Resource(callback);
  r->createProperty("value", Property{false, false});
  r->createProperty("inputURI", Property{std::bind(inputURIUpdated, r, "inputURI", std::placeholders::_1, std::placeholders::_2), true});
  r->createProperty("inputValue", Property{false, false});
  return r;
}

void notResourceUpdated(Resource* resource, const std::string& propertyName) {
  if (propertyName != "inputValue") return;

  try {
    bool value;
    CoAP::from_json(resource->readProperty("inputValue"), value);
    resource->setProperty("value", CoAP::to_json(!value));
  }
  catch (std::runtime_error& e) {

  }
}

std::unique_ptr<Resource> notResourceFactory() {
  return std::unique_ptr<Resource>(newNotResource(notResourceUpdated));
}
