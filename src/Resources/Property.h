/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <coap/Notifications.h>

#include <list>
#include <memory>
#include <string>

class Property {
  const bool writeable_;
  const bool persistent_{false};
  std::string value_;
  std::list<std::weak_ptr<CoAP::Notifications>> observer_;
  std::function<void(const std::string&, const std::string&)> onUpdate_;

 public:
  explicit Property(bool writeable, bool persitent) : writeable_(writeable), persistent_(persitent) { }
  explicit Property(std::function<void(const std::string&, const std::string&)> onUpdate, bool persistent)
      : writeable_(true), persistent_(persistent), onUpdate_(onUpdate) { }

  std::string read() const { return value_; }

  void update(const std::string& value);

  void set(const std::string& value);

  // TODO: Replace usage by function below
  void subscribe(std::weak_ptr<CoAP::Notifications> observer) {
    observer_.emplace_back(observer);
  }

  std::shared_ptr<CoAP::Notifications> subscribe();

  bool isWriteable() const { return writeable_; }

  bool isPersistent() const { return persistent_; }
};
