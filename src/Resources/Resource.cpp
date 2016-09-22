/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Resource.h"

void inputCountUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
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
      std::string uri = "input" + std::to_string(index) + "URI";
      resource->createProperty(uri, Property{std::bind(inputURIUpdated, resource, uri, std::placeholders::_1, std::placeholders::_2)});
      resource->createProperty("input" + std::to_string(index) + "Value", Property{false});
    }
  }
}

std::list<std::string> Resource::read() {
  std::list<std::string> content;

  for (auto& p : properties_) {
    content.push_back(p.first);
  }

  return content;
}

std::string Resource::readProperty(const std::string& name) {
  auto it = properties_.find(name);

  if (it == properties_.end()) throw std::runtime_error("Property not found");

  return it->second.read();
}

void Resource::updateProperty(const std::string& name, const std::string& value) {
  auto it = properties_.find(name);

  if (it == properties_.end()) throw std::runtime_error("Property not found");

  it->second.update(value);
  if (onPropertyChanged_) onPropertyChanged_(this, name);
}

void Resource::subscribeProperty(const std::string& name, std::weak_ptr<CoAP::Notifications> notifications) {
  auto it = properties_.find(name);

  if (it == properties_.end()) throw std::runtime_error("Property not found");

  it->second.subscribe(notifications);
}

void Resource::createProperty(const std::string name, Property property) {
  if (properties_.find(name) != properties_.end()) throw std::runtime_error("Property exists already");

  properties_.emplace(name, property);
}

void Resource::deleteProperty(const std::string name) {
  if (properties_.find(name) == properties_.end()) throw std::runtime_error("Property does not exist");

  properties_.erase(name);
}

void Resource::setProperty(const std::string& name, const std::string& value) {
  auto it = properties_.find(name);

  if (it == properties_.end()) throw std::runtime_error("Property not found");

  it->second.set(value);
  if (onPropertyChanged_) onPropertyChanged_(this, name);
}

