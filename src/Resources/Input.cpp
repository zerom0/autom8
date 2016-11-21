/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Input.h"

#include "Resource.h"

Resource* newInputResource() {
  auto resource = new Resource(nullptr);
  resource->createProperty("value", Property{true});
  return resource;
}

std::unique_ptr<Resource> inputResourceFactory() {
  return std::unique_ptr<Resource>(newInputResource());
}