/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Property.h"

#include <CoAP/Optional.h>

#include <map>
#include <string>

class Resource;
using ResourceChangedCallback = std::function<void(Resource*, const std::string&)>;

void inputURIUpdated(Resource* resource, const std::string& propertyName, const std::string& oldURI, const std::string& newURI);

void inputCountUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue);

class Resource {
  std::map<std::string, Property> properties_;
  ResourceChangedCallback onPropertyChanged_;

 public:
  /**
   * Creates a new resource with an optional observer that is being called when a
   * property's value changed.
   *
   * @param onPropertyChanged  Optional observer
   */
  explicit Resource(ResourceChangedCallback onPropertyChanged) : onPropertyChanged_(onPropertyChanged) { }

  /**
   * Returns a list of the resources properties.
   *
   * @return List of properties.
   */
  std::list<std::string> read() const;

  /**
   * Adds a property to the resource.
   *
   * @param name      Name of the property
   * @param property  Property to be added
   *
   * @throws runtime_error if a property with the given name already exists.
   */
  void createProperty(const std::string name, Property property);

  /**
   * Removes a property from the resource.
   *
   * @param name  Name of the property
   *
   * @throws runtime_error if the property does not exist.
   */
  void deleteProperty(const std::string name);

  Property* getProperty(const std::string& name);
  const Property* getProperty(const std::string& name) const;

  void setProperty(const std::string& name, const std::string& value);

  std::string to_json() const;
};

inline std::string getValueOr(const std::map<std::string, std::string>& values, const std::string& key, const std::string& alternative) {
  auto it = values.find(key);
  return (it != end(values)) ? it->second : alternative;
}
