/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "I2C.h"

#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#endif

static const char* filename = "/dev/i2c-1";

void I2C::write_byte(uint8_t byte) {
  int file;

  if((file = open(filename, O_RDWR))< 0 ) return;

#ifdef __linux__
  if (ioctl(file, I2C_SLAVE, address_) < 0) return;

  if (write(file, &byte, 1) != 1) return;
#endif

  close(file);
}

uint8_t I2C::read_byte() {
  uint8_t data{0};

  int file;

  if((file = open(filename, O_RDWR))< 0 ) return 0;
#ifdef __linux__

  if (ioctl(file, I2C_SLAVE, address_) < 0) return 0;

  if (read(file, &data, 1) != 1) return 0;
#endif

  close(file);

  return data;
}
