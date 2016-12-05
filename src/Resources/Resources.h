/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Resource.h"

#include <string>
#include <map>

class Path;

using ResourceFactory = std::map<std::string, std::function<std::unique_ptr<Resource>(std::map<std::string, std::string>)>>;

using ResourceMap = std::map<std::string, std::unique_ptr<Resource>>;

class Resources {
  ResourceMap resources_;
  ResourceFactory& factory_;

 public:
  Resources(ResourceFactory& factory) : factory_(factory) { }

  /**
   * Returns a RestResponse with the resources filtered by the path of the resource where the
   * function is called upon. The response has the content type application/link-format.
   *
   * @param path        The path on which the request was executed on.
   * @return            RestResponse with the filtered resources in application/link-format.
   */
  CoAP::RestResponse listResources(const Path& path);

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
   * @param path      Path on which the function was called.
   * @return RestResponse with the resource properties in application/link-format.
   */
  CoAP::RestResponse readResource(const Path& path);

  /**
   * Removes a resource from the given resource container.
   * The path on which the function was called determines the resource to be removed.
   *
   * @param path      Path on which the function was called.
   * @return RestResponse with indication of success or failure.
   */
  CoAP::RestResponse deleteResource(const Path& path);

  CoAP::RestResponse readProperty(const Path& p);

  CoAP::RestResponse
  updateProperty(const Path& p, const std::string& value);

  CoAP::RestResponse observeProperty(const Path& p, std::weak_ptr<CoAP::Notifications> observer);

  std::string to_json();

  void createResourcesFromJSON(const std::string& data);
};
