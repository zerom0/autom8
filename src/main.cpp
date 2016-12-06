/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Resources/And.h"
#include "Resources/IO.h"
#include "Resources/Not.h"
#include "Resources/Or.h"
#include "Resources/Resources.h"

#include <coap/CoAP.h>
#include <coap/json.h>
#include <coap/URI.h>

#include <functional>
#include <iostream>

using std::bind;
using std::map;
using std::placeholders::_1;
using std::placeholders::_2;
using std::ref;
using std::shared_ptr;
using std::string;

shared_ptr<CoAP::IMessaging> messaging;

map<string, shared_ptr<CoAP::Notifications>> activeNotifications;

/**
 * Handler for URI properties that subscribes to notifications for the resource specified by newURI
 * and updates the cached values with the values from the received notifications.
 *
 * @param resource      Resource for which the handler was called
 * @param propertyName  Property for which the handler was called
 * @param oldURI        URI of the old observed resource
 * @param newURI        URI of the new observed resource
 */
void inputURIUpdated(Resource* resource, const string& propertyName, const string& oldURI, const string& newURI) {
  // TODO: URI::fromString cannot handle empty strings -> Bang
  auto theUri = newURI.empty() ? URI{} : URI::fromString(newURI);

  if (newURI.empty()) return;

  auto client = messaging->getClientFor(theUri.getServer().c_str(), theUri.getPort());
  activeNotifications[newURI] = client.OBSERVE(theUri.getPath());
  string valuePropertyName = propertyName.substr(0, propertyName.length() - 3) + "Value";

  // TODO: Unregister from old URI
  // TODO: use property instead of resource if possible
  activeNotifications[newURI]->subscribe([resource, valuePropertyName](const CoAP::RestResponse& response) {
    resource->setProperty(valuePropertyName, response.payload());
  });
}

string fromPersistence =
    "{"
        "\"/in/fenster\":{\"inputURI\":\"\"},"
        "\"/in/tuere\":{\"inputURI\":\"\"},"
        "\"/and/all_closed\":{"
            "\"input0URI\":\"coap://127.0.0.1:5683/in/fenster/value\","
            "\"input1URI\":\"coap://127.0.0.1:5683/in/tuere/value\","
            "\"inputCount\":\"2\""
        "},"
        "\"/not/alarm\":{\"inputURI\":\"coap://127.0.0.1:5683/and/all_closed/value\"}"
    "}";

int main() {
  messaging = CoAP::newMessaging();

  ResourceFactory resourceFactory = {
      { "and", andResourceFactory },
      { "in",  ioResourceFactory },
      { "not", notResourceFactory },
      { "or",  orResourceFactory },
      { "out", ioResourceFactory },
  };

  Resources resources(resourceFactory);
  resources.createResourcesFromJSON(fromPersistence);

  const std::map<std::string, std::string> noValues;

  messaging->requestHandler()
      .onUri("/.well-known/core")
          .onGet(bind(&Resources::listResources, &resources, _1))
      .onUri("/?") // Resources
          .onGet(bind(&Resources::listResources, &resources, _1))
          .onPost(bind(&Resources::createResource, &resources, _1, _2, noValues))
      .onUri("/?/?") // Resource
          .onGet(bind(&Resources::readResource, &resources, _1))
          .onDelete(bind(&Resources::deleteResource, &resources, _1))
      .onUri("/?/?/?") // Property
          .onGet(bind(&Resources::readProperty, &resources, _1))
          .onPut(bind(&Resources::updateProperty, &resources, _1, _2))
          .onObserve(bind(&Resources::observeProperty, &resources, _1, _2));

  std::string persistence;

  int i = 0;
  for(;;) {
    messaging->loopOnce();
    i++;
    if (i%100) {
      std::string temp = resources.to_json();
      if (temp != persistence) {
        persistence = temp;
        std::cout << persistence << std::endl;
      }
    }
  }
}
