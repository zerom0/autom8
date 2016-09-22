/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Property.h"

#include <CoAP.h>

void Property::update(const std::string& value) {
  if (not isWriteable()) throw std::runtime_error("Property is read only");

  auto oldValue = value_;
  value_ = value;
  if (onUpdate_) onUpdate_(oldValue, value);
  for (auto& observer : observer_) {
    auto o = observer.lock();
    if (o) o->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(value));
  }
}

std::shared_ptr<CoAP::Notifications> Property::subscribe() {
  auto notifications = std::make_shared<CoAP::Notifications>();
  observer_.emplace_back(notifications);
  return notifications;
}
