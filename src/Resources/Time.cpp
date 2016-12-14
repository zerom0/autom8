/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Time.h"

#include "Resource.h"

#include <coap/Logging.h>

#include <regex>

using std::placeholders::_1;
using std::placeholders::_2;

SETLOGLEVEL(LLDEBUG)

Resource* newTimeResource(InputValueUpdated callback, const std::map<std::string, std::string>& values) {
  TLOG << "newTimeResource()\n";

  auto r = new Resource();
  r->createProperty("value", std::bind(callback, r, "value", _1, _2), Property::Persistent);

  for (auto it = begin(values); it != end(values); ++it) r->getProperty(it->first)->setValue(it->second);

  return r;
}

void timeResourceUpdated(Resource* resource, const std::string& propertyName, const std::string& oldValue, const std::string& newValue) {
  DLOG << "timeResourceUpdated(..., " << propertyName << ", " << oldValue << ", " << newValue << ")\n";

  if (propertyName != "value") return;

  auto time = Time::from_string(newValue);

  resource->getProperty("value")->setValue(time.to_string(), true);
}

std::unique_ptr<Resource> timeResourceFactory(const std::map<std::string, std::string>& values) {
  return std::unique_ptr<Resource>(newTimeResource(timeResourceUpdated, values));
}


Time Time::from_string(const std::string& text) {
  auto pattern = std::regex{"(\\d{0,2})(:(\\d{0,2})(:(\\d{0,2}))?)?"};
  auto match = std::smatch{}; // match;
  std::regex_match(text, match, pattern);

  auto mcount = match.size();
  DLOG << mcount << '\n';

  auto time = Time{};

  auto shours   = (mcount >= 2) ? std::string(match[1]) : "";
  shours = shours.empty() ? "0" : shours;
  time.hours_   = std::stoul(shours);

  auto sminutes = (mcount >= 4) ? std::string(match[3]) : "";
  sminutes = sminutes.empty() ? "0" : sminutes;
  time.minutes_ = std::stoul(sminutes);

  auto sseconds = (mcount >= 6) ? std::string(match[5]) : "";
  sseconds = sseconds.empty() ? "0" : sseconds;
  time.seconds_ = std::stoul(sseconds);

  if (time.hours_ > 23 || time.minutes_ > 59 || time.seconds_ > 59) {
    DLOG << "Invalid time " << text << '\n';
    time.hours_ = 0;
    time.minutes_ = 0;
    time.seconds_ = 0;
  }

  return time;
}
std::string Time::to_string() const {
  return      ((hours_ < 10) ? "0" : "") + std::to_string(hours_)
      + ':' + ((minutes_ < 10) ? "0" : "") + std::to_string(minutes_)
      + ':' + ((seconds_ < 10) ? "0" : "") + std::to_string(seconds_);
}
