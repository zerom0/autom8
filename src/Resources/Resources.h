/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "Resource.h"

#include <string>
#include <map>

class Path;

using ResourceFactory = std::map<std::string, std::function<std::unique_ptr<Resource>(std::map<std::string, std::string>)>>;

using Resources = std::map<std::string, std::unique_ptr<Resource>>;

/**
 * Returns a RestResponse with the resources filtered by the path of the resource where the
 * function is called upon. The response has the content type application/link-format.
 *
 * @param factory     The resource factory to get the URIs to the element roots.
 * @param resources   The container with all existing resources.
 * @param path        The path on which the request was executed on.
 * @return            RestResponse with the filtered resources in application/link-format.
 */
CoAP::RestResponse listResources(ResourceFactory& factory, Resources& resources, const Path& path);

/**
 * Creates a new resource with the given factory and stores it in the given resource container.
 * The path on which the function was called determines the type of the resource to be created.
 *
 * @param factory   Factory to create the resource.
 * @param resources Resource container to receive the newly created resource.
 * @param path      Path on which the function was called.
 * @param name     Name of the newly created resource.
 * @return RestResponse with indication of success or failure.
 */
CoAP::RestResponse createResource(ResourceFactory& factory, Resources& resources, const Path& path, const std::string& name, const std::map<std::string, std::string>& values);

/**
 * Returns a RestResponse with the properties of the resource in application/link-format.
 * The path on which the function was called determines the resource that shall be considered.
 *
 * @param resources Resource container with the resources to consider.
 * @param path      Path on which the function was called.
 * @return RestResponse with the resource properties in application/link-format.
 */
CoAP::RestResponse readResource(Resources& resources, const Path& path);

/**
 * Removes a resource from the given resource container.
 * The path on which the function was called determines the resource to be removed.
 *
 * @param resources Resource container with the resources.
 * @param path      Path on which the function was called.
 * @return RestResponse with indication of success or failure.
 */
CoAP::RestResponse deleteResource(Resources& resources, const Path& path);

CoAP::RestResponse readProperty(Resources& resources, const Path& p);

CoAP::RestResponse updateProperty(Resources& resources, const Path& p, const std::string& value);

CoAP::RestResponse observeProperty(Resources& resources, const Path& p, std::weak_ptr<CoAP::Notifications> observer);

std::string to_json(const Resources& resources);

void createResourcesFromJSON(ResourceFactory& factory, Resources& resources, const std::string& data);
