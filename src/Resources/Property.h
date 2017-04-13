/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <coap/Notifications.h>

#include <list>
#include <memory>
#include <string>

class Property {
 public:
  enum Access {
    ReadWrite,
    ReadOnly
  };

  enum Livetime {
    Persistent,
    Volatile
  };

 private:
  const Access access_{ReadWrite};

  const Livetime livetime_{Volatile};

  /// The property's current value
  std::string value_;

  /// Observers to be notified when the property's value changes.
  std::list<std::weak_ptr<CoAP::Notifications>> observer_;

  /// Handler to be called when the property's value changes.
  std::function<void(const std::string&, const std::string&)> onUpdate_;

  /// When the property is based on notifications it manages the livetime of its notifications.
  std::shared_ptr<CoAP::Notifications> notifications_;

 public:
  Property(Access access, Livetime livetime) : access_(access), livetime_(livetime) { }

  Property* onUpdate(std::function<void(const std::string&, const std::string&)> updateHandler) {
    onUpdate_ = updateHandler;
    return this;
  }

  std::string getValue() const;

  void setValue(const std::string& value, bool force = false);

  // TODO: Replace usage by function below
  void subscribe(std::weak_ptr<CoAP::Notifications> observer) {
    observer_.emplace_back(observer);
  }

  std::shared_ptr<CoAP::Notifications> subscribe();

  bool isWriteable() const { return access_ == ReadWrite; }

  bool isPersistent() const { return livetime_ == Persistent; }

  void setNotifications(std::shared_ptr<CoAP::Notifications> notifications);

  void clearNotifications();
};
