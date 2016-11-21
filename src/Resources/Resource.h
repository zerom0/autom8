/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Property.h"

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
  std::list<std::string> read();

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

  /**
   * Returns the value of a property.
   *
   * @param name  Name of the property
   * @return  Value of the property
   *
   * @throws runtime_error if the property does not exist.
   */
  std::string readProperty(const std::string& name);

  /**
   * Changes the value or a property.
   *
   * @param name   Name of the property
   * @param value  New value for the property
   *
   * @throws runtime_error if the property does not exist.
   */
  void updateProperty(const std::string& name, const std::string& value);

  /**
   * Creates a subscription for future changes of a property.
   *
   * @param name           Name of the property
   * @param notifications  Future notifications of property value changes
   * @return      A shared object with future value changed notifications
   *
   * @throws runtime_error if the property does not exist.
   */
  void subscribeProperty(const std::string& name, std::weak_ptr<CoAP::Notifications> notifications);

  void setProperty(const std::string& name, const std::string& value);
};
