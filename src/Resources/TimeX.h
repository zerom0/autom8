/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <chrono>
#include <map>
#include <memory>

class Resource;

/**
 * Factory for a new resource of type Input/Output.
 *
 * @return Ownership of heap allocated resource.
 */
std::unique_ptr<Resource> timeResourceFactory(const std::map<std::string, std::string>& values);

class Time {
  unsigned hours_{0};
  unsigned minutes_{0};
  unsigned seconds_{0};

 public:
  static Time from_string(const std::string& text);
  std::string to_string() const;
};
