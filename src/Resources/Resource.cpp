/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Resource.h"

#include <coap/json.h>
#include <coap/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;

SETLOGLEVEL(LLDEBUG)

void inputCountUpdated(Resource* resource,
                       InputValueUpdated callback,
                       const std::string& oldValue,
                       const std::string& newValue) {
  TLOG << "inputCountUpdated(..., ..., " << oldValue << ", " << newValue << ")\n";

  auto oldCount = 0UL;
  try {
    oldCount = std::stoul(oldValue);
  } catch (std::exception& e) {
    // The property might not be initialized the first time update is called
  }
  auto newCount = std::stoul(newValue);

  if (newCount < oldCount) {
    for (auto index = newCount; index < oldCount; ++index) {
      resource->deleteProperty("input" + std::to_string(index) + "URI");
      resource->deleteProperty("input" + std::to_string(index) + "Value");
    }
  } else {
    for (auto index = oldCount; index < newCount; ++index) {
      std::string valuePropertyName = "input" + std::to_string(index) + "Value";
      auto value = resource->createProperty(valuePropertyName, std::bind(callback, resource, valuePropertyName, _1, _2), Property::Volatile);
      std::string uriPropertyName = "input" + std::to_string(index) + "URI";
      resource->createProperty(uriPropertyName, std::bind(inputURIUpdated, value, _1, _2), Property::Persistent);
    }
  }
}

std::list<std::string> Resource::read() const {
  TLOG << "Resource::read()\n";

  std::list<std::string> content;

  for (auto& p : properties_) {
    content.push_back(p.first);
  }

  return content;
}

Property* Resource::createProperty(const std::string name, Property property) {
  TLOG << "Resource::createProperty(" << name << ", ...)\n";
  if (getProperty(name)) throw std::runtime_error("Property exists already");
  auto result = properties_.emplace(name, property);
  return &(result.first->second);
}

void Resource::deleteProperty(const std::string name) {
  TLOG << "Resource::deleteProperty(" << name << ")\n";
  if (!getProperty(name)) throw std::runtime_error("Property does not exist");
  properties_.erase(name);
}

Property* Resource::getProperty(const std::string& name) {
  TLOG << "Resource::getProperty(" << name << ")\n";
  auto it = properties_.find(name);
  return (it != end(properties_)) ? &(it->second) : nullptr;
}

const Property* Resource::getProperty(const std::string& name) const{
  TLOG << "Resource::getProperty(" << name << ")\n";
  auto it = properties_.find(name);
  return (it != end(properties_)) ? &(it->second) : nullptr;
}

std::string Resource::to_json() const {
  TLOG << "Resource::to_json()\n";
  std::string json;
  auto first = true;

  for (const auto& it : properties_) {
    if (it.second.isPersistent()) {
      if (first) first = false;
      else json += ",";
      json += CoAP::to_json(it.first) + ":" + CoAP::to_json(it.second.getValue());
    }
  }
  return "{" + json + "}";
}
