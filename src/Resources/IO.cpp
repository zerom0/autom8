/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "IO.h"

#include "Resource.h"

Resource* newIOResource(ResourceChangedCallback callback) {
  auto r = new Resource(callback);
  r->createProperty("value", Property{true});
  r->createProperty("inputURI", Property{std::bind(inputURIUpdated, r, "inputURI", std::placeholders::_1, std::placeholders::_2)});
  r->createProperty("inputValue", Property{false});
  return r;
}

void ioResourceUpdated(Resource* resource, const std::string& propertyName) {
  if (propertyName != "inputValue") return;

  resource->setProperty("value", resource->readProperty("inputValue"));
}

std::unique_ptr<Resource> ioResourceFactory() {
  return std::unique_ptr<Resource>(newIOResource(ioResourceUpdated));
}