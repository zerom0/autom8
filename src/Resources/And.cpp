/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "And.h"

#include "Resource.h"

#include <coap/json.h>
#include <coap/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;

SETLOGLEVEL(LLWARNING)

Resource* newAndResource(InputValueUpdated callback, const std::map<std::string, std::string>& values) {
  TLOG << "newAndResource()\n";

  auto r = new Resource();
  r->createProperty("value", Property::ReadOnly, Property::Volatile);

  auto inputCount = r->createProperty("inputCount", std::bind(inputCountUpdated, r, callback, _1, _2), Property::Persistent);
  inputCount->setValue(getValueOr(values, "inputCount", "2"));

  r->init(values);

  return r;
}

void andResourceUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
  DLOG << "andResourceUpdated(..., " << propertyName << ", " << oldValue << ", " << newValue << ")\n";

  if (propertyName.find("input") != 0) return;

  try {
    auto value = reduceInputValues(resource, true, [](bool l, bool r){return l && r;});
    resource->getProperty("value")->setValue(CoAP::to_json(value), true);
  }
  catch (std::runtime_error& e) {
    ELOG << e.what() << '\n';
  }
}

std::unique_ptr<Resource> andResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newAndResource(andResourceUpdated, values));
}
