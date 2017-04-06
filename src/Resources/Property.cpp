/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "Property.h"

#include <coap/CoAP.h>
#include <coap/Logging.h>

SETLOGLEVEL(LLWARNING)

std::string Property::getValue() const {
  TLOG << "Property::getValue() -> " << value_ << "\n";
  return value_;
}

void Property::setValue(const std::string& value, bool force) {
  TLOG << "Property::setValue(" << value << ", " << (force ? "true":"false") << ")\n";

  if (not force and not isWriteable()) throw std::runtime_error("Property is read only");

  if (value == value_) return;

  auto oldValue = value_;
  value_ = value;
  if (onUpdate_) onUpdate_(oldValue, value);
  for (auto it = begin(observer_); it != end(observer_);) {
    auto o = it->lock();
    if (o) {
      DLOG << "Observer notified\n";
      o->onNext(CoAP::RestResponse().withCode(CoAP::Code::Content).withPayload(value));
      ++it;
    }
    else {
      DLOG << "Inactive observer removed\n";
      observer_.erase(it++);
    }
  }
}

std::shared_ptr<CoAP::Notifications> Property::subscribe() {
  auto notifications = std::make_shared<CoAP::Notifications>();
  observer_.emplace_back(notifications);
  return notifications;
}

void Property::setNotifications(std::shared_ptr<CoAP::Notifications> notifications) {
  notifications_ = notifications;
  notifications_->subscribe([this](const CoAP::RestResponse& response) {
    setValue(response.payload(), true);
  });
}

void Property::clearNotifications() {
  notifications_.reset();
}
