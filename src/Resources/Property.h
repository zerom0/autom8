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
  std::string value_;
  std::list<std::weak_ptr<CoAP::Notifications>> observer_;
  std::function<void(const std::string&, const std::string&)> onUpdate_;

 public:
  Property(Access access, Livetime livetime) : access_(access), livetime_(livetime) { }

  Property(std::function<void(const std::string&, const std::string&)> onUpdate, Livetime livetime)
      : livetime_(livetime), onUpdate_(onUpdate) {
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
};
