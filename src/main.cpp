/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Resources/And.h"
#include "Resources/History.h"
#include "Resources/IO.h"
#include "Resources/Not.h"
#include "Resources/Or.h"
#include "Resources/Relation.h"
#include "Resources/Resources.h"
#include "Resources/TimeX.h"

#include <coap/CoAP.h>
#include <coap/json.h>
#include <coap/Logging.h>
#include <coap/URI.h>

#include <fstream>
#include <functional>
#include <iostream>

SETLOGLEVEL(LLDEBUG)

using std::bind;
using std::map;
using std::placeholders::_1;
using std::placeholders::_2;
using std::ref;
using std::shared_ptr;
using std::string;

shared_ptr<CoAP::IMessaging> messaging;

/**
 * Handler for URI properties that subscribes to notifications for the resource specified by newURI
 * and updates the cached values with the values from the received notifications.
 *
 * @param property  Property for which the handler was called
 * @param oldURI    URI of the old observed resource
 * @param newURI    URI of the new observed resource
 */
void inputURIUpdated(Property* property, const string& oldURI, const string& newURI) {
  auto theUri = URI::fromString(newURI);

  if (theUri.isValid()) {
    auto client = messaging->getClientFor(theUri.getServer().c_str(), theUri.getPort());
    property->setNotifications(client.OBSERVE(theUri.getPath()));
    DLOG << "Subscribed on " << newURI << "\n";
  }
  else {
    property->clearNotifications();
  }
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

void onResourcesModified(const Resources& resources) {
  static std::string persistence;
  string temp = resources.to_json();
  if (temp != persistence) {
    persistence = temp;
    auto file = std::ofstream("autom8.dat");
    file << persistence;
  }
}

int main() {
  messaging = CoAP::newMessaging();

  ResourceFactory resourceFactory = {
      { "and", andResourceFactory },
      { "history", historyResourceFactory },
      { "in",  ioResourceFactory },
      { "not", notResourceFactory },
      { "or",  orResourceFactory },
      { "out", ioResourceFactory },
      { "relation", relationResourceFactory },
//      { "time", timeResourceFactory },
  };

  Resources resources(resourceFactory, onResourcesModified);
  auto file = std::ifstream("autom8.dat");
  if (file.is_open()) {
    getline(file, fromPersistence);
  }
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

  for(;;) {
    messaging->loopOnce();
  }
}
