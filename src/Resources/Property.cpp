/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Property.h"

#include <coap/CoAP.h>
#include <coap/Logging.h>

SETLOGLEVEL(LLDEBUG)

std::string Property::getValue() const {
  DLOG << "Property::getValue() -> " << value_ << "\n";
  return value_;
}

void Property::setValue(const std::string& value, bool force) {
  DLOG << "Property::setValue(" << value << ", " << (force ? "true":"false") << ")\n";

  if (not force and not isWriteable()) throw std::runtime_error("Property is read only");

  auto oldValue = value_;
  value_ = value;
  if (onUpdate_) onUpdate_(oldValue, value);
  for (auto& observer : observer_) {
    auto o = observer.lock();
    DLOG << (o ? "in": "") << "active observer notified\n";
    if (o) o->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(value));
  }
}

std::shared_ptr<CoAP::Notifications> Property::subscribe() {
  auto notifications = std::make_shared<CoAP::Notifications>();
  observer_.emplace_back(notifications);
  return notifications;
}
