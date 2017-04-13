/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "History.h"

#include "Resource.h"

#include <coap/json.h>
#include <coap/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;

SETLOGLEVEL(LLDEBUG)

/*
 * The history resource stores the last values of type double of another resource.
 *
 * inputURI   rw   Path to the resource that will provide the values (double)
 * inputValue r    Last value (double) received from the observed resource
 * values     r    Last values (List<double>) received from the observed resource
 * maxSize    r/w  Number of values to be stored
 * min        r    Min value in the list
 * max        r    Max value in the list
 * average    r    Average value of the list
 */

Resource* newHistoryResource(InputValueUpdated callback, const std::map<std::string, std::string>& values) {
  TLOG << "newHistoryResource()\n";

  auto r = new Resource();
  r->createProperty("values", Property::ReadOnly, Property::Volatile)
      ->setValue("[]", true);

  r->createProperty("maxSize", Property::ReadWrite, Property::Persistent)
      ->setValue("10", true);

  r->createProperty("min", Property::ReadOnly, Property::Volatile);
  r->createProperty("max", Property::ReadOnly, Property::Volatile);
  r->createProperty("average", Property::ReadOnly, Property::Volatile);

  auto value = r->createProperty("inputValue", Property::ReadWrite, Property::Volatile)
      ->onUpdate(std::bind(callback, r, "inputValue", _1, _2));

  r->createProperty("inputURI", Property::ReadWrite, Property::Persistent)
      ->onUpdate(std::bind(inputURIUpdated, value, _1, _2));

  r->init(values);

  return r;
}

void historyResourceUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
  DLOG << "historyResourceUpdated(..., " << propertyName << ", " << oldValue << ", " << newValue << ")\n";

  if (propertyName != "inputValue") return;

  try {
    std::list<double> values;
    CoAP::from_json(resource->getProperty("values")->getValue(), values);

    double value;
    CoAP::from_json(newValue, value);
    values.emplace_back(value);

    int maxSize;
    CoAP::from_json(resource->getProperty("maxSize")->getValue(), maxSize);
    while (values.size() >= maxSize) values.pop_front();

    double minValue = 0;
    double maxValue = 0;
    double averageValue = 0;
    bool first = true;
    for (auto v : values) {
      if (first) {
        minValue = v;
        maxValue = v;
        averageValue = v / values.size();
        first = false;
      } else {
        minValue = std::min(minValue, v);
        maxValue = std::max(maxValue, v);
        averageValue += v / values.size();
      }
    }

    resource->getProperty("min")->setValue(CoAP::to_json(minValue), true);
    resource->getProperty("max")->setValue(CoAP::to_json(maxValue), true);
    resource->getProperty("average")->setValue(CoAP::to_json(averageValue), true);
    resource->getProperty("values")->setValue(CoAP::to_json(values), true);
  }
  catch (std::runtime_error& e) {

  }
}

std::unique_ptr<Resource> historyResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newHistoryResource(historyResourceUpdated, values));
}
