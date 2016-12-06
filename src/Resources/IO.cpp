/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "IO.h"

#include "Resource.h"
#include <coap/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;

SETLOGLEVEL(LLDEBUG)

Resource* newIOResource(InputValueUpdated callback, const std::map<std::string, std::string>& values) {
  DLOG << "newIOResource()\n";

  auto r = new Resource();
  r->createProperty("value", true, false);

  auto value = r->createProperty("inputValue", std::bind(callback, r, "inputValue", _1, _2), false);
  r->createProperty("inputURI", std::bind(inputURIUpdated, value, _1, _2), true);

  for (auto it = begin(values); it != end(values); ++it) r->getProperty(it->first)->setValue(it->second);

  return r;
}

void ioResourceUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
  DLOG << "ioResourceUpdated(..., " << propertyName << ", " << oldValue << ", " << newValue << ")\n";

  if (propertyName != "inputValue") return;

  resource->getProperty("value")->setValue(newValue, true);
}

std::unique_ptr<Resource> ioResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newIOResource(ioResourceUpdated, values));
}