/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Or.h"

#include "Resource.h"

#include <coap/json.h>
#include <coap/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;

SETLOGLEVEL(LLDEBUG)

Resource* newOrResource(InputValueUpdated callback, const std::map<std::string, std::string>& values) {
  DLOG << "newOrResource()\n";

  auto r = new Resource();
  r->createProperty("value", false, false);
  r->createProperty("inputCount", std::bind(inputCountUpdated, r, callback, std::placeholders::_1, std::placeholders::_2), true);
  r->getProperty("inputCount")->setValue(getValueOr(values, "inputCount", "2"));

  for (auto it = begin(values); it != end(values); ++it) r->getProperty(it->first)->setValue(it->second);

  return r;
}

void orResourceUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
  DLOG << "orResourceUpdated(..., " << propertyName << ", " << oldValue << ", " << newValue << ")\n";

  if (propertyName.find("input") != 0) return;

  try {
    unsigned count;
    CoAP::from_json(resource->getProperty("inputCount")->getValue(), count);

    auto value = false;

    for (unsigned i = 0; i < count; ++i) {
      bool inputValue;
      CoAP::from_json(resource->getProperty("input" + std::to_string(i) + "Value")->getValue(), inputValue);
      value |= inputValue;
    }

    resource->getProperty("value")->setValue(CoAP::to_json(value), true);
  }
  catch (std::runtime_error& e) {
    ELOG << e.what() << '\n';
  }
}

std::unique_ptr<Resource> orResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newOrResource(orResourceUpdated, values));
}
