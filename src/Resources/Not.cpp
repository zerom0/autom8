/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Not.h"

#include "Resource.h"

#include <coap/json.h>
#include <coap/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;

SETLOGLEVEL(LLWARNING)

Resource* newNotResource(InputValueUpdated callback, const std::map<std::string, std::string>& values) {
  TLOG << "newNotResource()\n";

  auto r = new Resource();
  r->createProperty("value", Property::ReadOnly, Property::Volatile);

  auto value = r->createProperty("inputValue", std::bind(callback, r, "inputValue", _1, _2), Property::Volatile);
  r->createProperty("inputURI", std::bind(inputURIUpdated, value, _1, _2), Property::Persistent);

  r->init(values);

  return r;
}

void notResourceUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
  DLOG << "notResourceUpdated(..., " << propertyName << ", " << oldValue << ", " << newValue << ")\n";

  if (propertyName != "inputValue") return;

  try {
    bool value;
    CoAP::from_json(newValue, value);
    resource->getProperty("value")->setValue(CoAP::to_json(!value), true);
  }
  catch (std::runtime_error& e) {

  }
}

std::unique_ptr<Resource> notResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newNotResource(notResourceUpdated, values));
}
