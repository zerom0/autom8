/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <Notifications.h>

#include <list>
#include <string>

class Property {
  const bool writeable_;
  std::string value_;
  std::list<std::weak_ptr<CoAP::Notifications>> observer_;
  std::function<void(const std::string&, const std::string&)> onUpdate_;

 public:
  explicit Property(bool writeable) : writeable_(writeable) { }
  explicit Property(std::function<void(const std::string&, const std::string&)> onUpdate)
      : writeable_(true), onUpdate_(onUpdate) { }

  std::string read() { return value_; }

  void update(const std::string& value);

  // TODO: Replace usage by function below
  void subscribe(std::weak_ptr<CoAP::Notifications> observer) {
    observer_.emplace_back(observer);
  }

  std::shared_ptr<CoAP::Notifications> subscribe();

  bool isWriteable() { return writeable_; }
};
