/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Resource.h"

#include <string>
#include <map>

class Path;

using ResourceFactory = std::map<std::string, std::function<std::unique_ptr<Resource>(std::map<std::string, std::string>)>>;


class Resources {
  using ResourceMap = std::map<std::string, std::unique_ptr<Resource>>;

  ResourceMap resources_;
  ResourceFactory factory_;

 public:
  /**
   * Creates the resource container with the given resource factory
   *
   * @param factory  Factory to create new resources
   */
  explicit Resources(ResourceFactory factory) : factory_(factory) { }

  /**
   * Returns a RestResponse with the resources filtered by the path of the resource where the
   * function is called upon. The response has the content type application/link-format.
   *
   * @param path        The path on which the request was executed on.
   * @return            RestResponse with the filtered resources in application/link-format.
   */
  CoAP::RestResponse listResources(const Path& path) const;

  /**
   * Creates a new resource with the given factory and stores it in the given resource container.
   * The path on which the function was called determines the type of the resource to be created.
   *
   * @param path      Path on which the function was called.
   * @param name     Name of the newly created resource.
   * @return RestResponse with indication of success or failure.
   */
  CoAP::RestResponse createResource(const Path& path,
                                    const std::string& name,
                                    const std::map<std::string, std::string>& values);

  /**
   * Returns a RestResponse with the properties of the resource in application/link-format.
   * The path on which the function was called determines the resource that shall be considered.
   *
   * @param path  Path of the resource.
   * @return RestResponse with the resource properties in application/link-format.
   */
  CoAP::RestResponse readResource(const Path& path) const;

  /**
   * Removes a resource from the given resource container.
   * The path on which the function was called determines the resource to be removed.
   *
   * @param path  Path of the resource.
   * @return RestResponse with indication of success or failure.
   */
  CoAP::RestResponse deleteResource(const Path& path);

  /**
   * Returns a RestResponse with the value of the property.
   *
   * @param path  Path of the property.
   * @return      RestResponse with the property value.
   */
  CoAP::RestResponse readProperty(const Path& path) const;

  /**
   * Update a property value with the given value.
   *
   * @param path    Path of the property
   * @param value   New value of the property
   * @return        RestResponse with a success indication.
   */
  CoAP::RestResponse updateProperty(const Path& path, const std::string& value);

  /**
   * Registers an observer for a given property.
   *
   * @param path      Path of the property.
   * @param observer  Future notifications of property value changes.
   * @return          RestResponse with the property value.
   */
  CoAP::RestResponse observeProperty(const Path& path, std::weak_ptr<CoAP::Notifications> observer);

  std::string to_json();

  void createResourcesFromJSON(const std::string& data);

 private:
  Resource* getResource(const Path& path);
  const Resource* getResource(const Path& path) const;

  Property* getProperty(const Path& path);
  const Property* getProperty(const Path& path) const;
};
