/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <cstdint>

class I2C {
  uint8_t address_;
 public:
  I2C(uint8_t address) : address_(address) {}

  void write_byte(uint8_t byte);
  uint8_t read_byte();
};
