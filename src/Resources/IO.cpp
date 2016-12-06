/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "IO.h"

#include "Resource.h"

Resource* newIOResource(ResourceChangedCallback callback, const std::map<std::string, std::string>& values) {
  auto r = new Resource(callback);
  r->createProperty("value", Property{true, false});
  r->createProperty("inputURI", Property{std::bind(inputURIUpdated, r, "inputURI", std::placeholders::_1, std::placeholders::_2), true});
  r->createProperty("inputValue", Property{false, false});

  for (auto it = begin(values); it != end(values); ++it) r->getProperty(it->first)->setValue(it->second);

  return r;
}

void ioResourceUpdated(Resource* resource, const std::string& propertyName) {
  if (propertyName != "inputValue") return;

  resource->setProperty("value", resource->getProperty("inputValue")->getValue());
}

std::unique_ptr<Resource> ioResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newIOResource(ioResourceUpdated, values));
}