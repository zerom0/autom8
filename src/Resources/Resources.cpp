/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Resources.h"

#include <coap/json.h>
#include <coap/Path.h>
#include <coap/RestResponse.h>
#include <iostream>

CoAP::RestResponse Resources::listResources(const Path& path) const {
  std::string response;

  std::string startPath = path.toString();

  if (startPath == "/.well-known/core") {
    for (auto& it : factory_) {
      response += "</" + it.first + ">;ct=40,\n";
    }
  }

  for (auto& it : resources_) {
    if (it.first.find(startPath) == 0) {
      response += "<" + it.first + ">;ct=40,\n";
    }
  }

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Content)
      .withContentFormat(40) // application/link-format
      .withPayload(response);
}

CoAP::RestResponse Resources::createResource(const Path& path,
                                             const std::string& name,
                                             const std::map<std::string, std::string>& values) {
  std::string resourceType = path.getPart(0);
  auto it = factory_.find(resourceType);
  if (it == end(factory_)) return CoAP::RestResponse().withCode(CoAP::Code::NotAcceptable);

  std::string uri = "/" + resourceType + "/" + name;
  resources_[uri] = it->second(values);

  resourcesModified_(*this);

  return CoAP::RestResponse().withCode(CoAP::Code::Created);
}

CoAP::RestResponse Resources::readResource(const Path& path) const {
  auto resource = getResource(path);
  if (!resource) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  std::string payload;
  for (auto& property : resource->read()) {
    payload += "<" + path.toString() + "/" + property + ">,";
  }

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Content)
      .withContentFormat(40)
      .withPayload(payload);
}

CoAP::RestResponse Resources::deleteResource(const Path& path) {
  auto it = resources_.find(path.toString());
  if (it == end(resources_)) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  resources_.erase(it);

  resourcesModified_(*this);

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Deleted);
}

CoAP::RestResponse Resources::readProperty(const Path& path) const {
  auto property = getProperty(path);
  if (!property) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Content)
      .withPayload(property->getValue());
}

CoAP::RestResponse Resources::updateProperty(const Path& path, const std::string& value) {
  auto property = getProperty(path);
  if (!property) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  property->setValue(value);

  resourcesModified_(*this);

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Changed);
}

CoAP::RestResponse
Resources::observeProperty(const Path& path, std::weak_ptr<CoAP::Notifications> observer) {
  auto property = getProperty(path);
  if (!property) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  property->subscribe(observer);

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Content)
      .withPayload(property->getValue());
}

std::string Resources::to_json() const {
  std::string json;
  auto first = true;
  for (auto& it : resources_) {
    if (first) first = false;
    else json += ",";
    // TODO: CoAP::to_json(key, value) plaziert "" um den value, auch wenn dieser schon json ist.
    json += CoAP::to_json(it.first) + ":" + it.second->to_json();
  }
  return "{" + json + "}";
}

void Resources::createResourcesFromJSON(const std::string& data) {
  std::map<std::string, std::map<std::string, std::string>> resourceMap;
  CoAP::from_json(data, resourceMap);

  for (auto resIt = begin(resourceMap); resIt != end(resourceMap); ++resIt) {
    Path p(resIt->first);
    auto path = Path("/" + p.getPart(0));
    createResource(path, p.getPart(1), resIt->second);
  }
}
Resource* Resources::getResource(const Path& path) {
  auto it = resources_.find("/" + path.getPart(0) + "/" + path.getPart(1));
  return (it != end(resources_)) ? it->second.get() : nullptr;
}

const Resource* Resources::getResource(const Path& path) const {
  auto it = resources_.find("/" + path.getPart(0) + "/" + path.getPart(1));
  return (it != end(resources_)) ? it->second.get() : nullptr;
}

Property* Resources::getProperty(const Path& path) {
  auto resource = getResource(path);
  auto property = resource ? resource->getProperty(path.getPart(2)) : nullptr;
  return property;
}

const Property* Resources::getProperty(const Path& path) const {
  auto resource = getResource(path);
  auto property = resource ? resource->getProperty(path.getPart(2)) : nullptr;
  return property;
}
