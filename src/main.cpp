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
  auto theUri = URI::fromString(newURI);
  auto client = messaging->getClientFor(theUri.getServer().c_str(), theUri.getPort());
  activeNotifications[newURI] = client.OBSERVE(theUri.getPath());
  string valuePropertyName = propertyName.substr(0, propertyName.length() - 3) + "Value";

  // TODO: Unregister from old URI
  activeNotifications[newURI]->subscribe([resource, valuePropertyName](const CoAP::RestResponse& response) {
    resource->setProperty(valuePropertyName, response.payload());
  });
}


int main() {
  messaging = CoAP::newMessaging();

  ResourceFactory resourceFactory = {
      { "and", andResourceFactory },
      { "in",  ioResourceFactory },
      { "not", notResourceFactory },
      { "or",  orResourceFactory },
      { "out", ioResourceFactory },
  };

  Resources resources;

  messaging->requestHandler()
      .onUri("/.well-known/core")
          .onGet(bind(listResources, ref(resourceFactory), ref(resources), _1))
      .onUri("/?") // Resources
          .onGet(bind(listResources, ref(resourceFactory), ref(resources), _1))
          .onPost(bind(createResource, ref(resourceFactory), ref(resources), _1, _2))
      .onUri("/?/?") // Resource
          .onGet(bind(readResource, ref(resources), _1))
          .onDelete(bind(deleteResource, ref(resources), _1))
      .onUri("/?/?/?") // Property
          .onGet(bind(readProperty, ref(resources), _1))
          .onPut(bind(updateProperty, ref(resources), _1, _2))
          .onObserve(bind(observeProperty, ref(resources), _1, _2));

  for(;;) {
    messaging->loopOnce();
  }
}
