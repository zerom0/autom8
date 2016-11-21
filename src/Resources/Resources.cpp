/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Resources.h"

#include <coap/Path.h>
#include <coap/RestResponse.h>

/**
 * Returns a RestResponse with the resources filtered by the path of the resource where the
 * function is called upon. The response has the content type application/link-format.
 *
 * @param factory     The resource factory to get the URIs to the element roots.
 * @param resources   The container with all existing resources.
 * @param path        The path on which the request was executed on.
 * @return            RestResponse with the filtered resources in application/link-format.
 */
CoAP::RestResponse listResources(ResourceFactory& factory, Resources& resources, const Path& path) {
  std::string response;

  std::string startPath = path.toString();

  if (startPath == "/.well-known/core") {
    for (auto& it : factory) {
      response += "</" + it.first + ">;ct=40,\n";
    }
  }

  for (auto& it : resources) {
    if (it.first.find(startPath) == 0) {
      response += "<" + it.first + ">;ct=40,\n";
    }
  }

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Content)
      .withContentFormat(40) // application/link-format
      .withPayload(response);
}

CoAP::RestResponse createResource(ResourceFactory& factory, Resources& resources, const Path& path, const std::string& name) {
  std::string resourceType = path.getPart(0);
  auto it = factory.find(resourceType);
  if (it == end(factory)) return CoAP::RestResponse().withCode(CoAP::Code::NotAcceptable);

  std::string uri = "/" + resourceType + "/" + name;
  resources[uri] = it->second();

  return CoAP::RestResponse().withCode(CoAP::Code::Created);
}

CoAP::RestResponse readResource(Resources& resources, const Path& path) {
  auto it = resources.find(path.toString());
  if (it == end(resources)) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  std::string payload;
  for (auto& property : it->second->read()) {
    payload += "<" + path.toString() + "/" + property + ">,";
  }

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Content)
      .withContentFormat(40)
      .withPayload(payload);
}

CoAP::RestResponse deleteResource(Resources& resources, const Path& path) {
  auto it = resources.find(path.toString());
  if (it == end(resources)) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  resources.erase(it);

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Deleted);
}

CoAP::RestResponse readProperty(Resources& resources, const Path& p) {
  auto it = resources.find("/" + p.getPart(0) + "/" + p.getPart(1));
  if (it == end(resources)) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Content)
      .withPayload(it->second->readProperty(p.getPart(2)));
}

CoAP::RestResponse updateProperty(Resources& resources, const Path& p, const std::string& value) {
  auto it = resources.find("/" + p.getPart(0) + "/" + p.getPart(1));
  if (it == end(resources)) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  it->second->updateProperty(p.getPart(2), value);

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Changed);
}

CoAP::RestResponse observeProperty(Resources& resources, const Path& p, std::weak_ptr<CoAP::Notifications> observer) {
  auto it = resources.find("/" + p.getPart(0) + "/" + p.getPart(1));
  if (it == end(resources)) return CoAP::RestResponse().withCode(CoAP::Code::NotFound);

  it->second->subscribeProperty(p.getPart(2), observer);

  return CoAP::RestResponse()
      .withCode(CoAP::Code::Content)
      .withPayload(it->second->readProperty(p.getPart(2)));
}
