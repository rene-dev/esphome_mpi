#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace pipsolar {

class Pipsolar : public uart::UARTDevice, public PollingComponent {
  void setup() override;
  void loop() override;
  void dump_config() override;
  void update() override;
  static const size_t READ_BUFFER_LENGTH = 256;  // maximum supported answer length
  uint8_t read_buffer[READ_BUFFER_LENGTH];
  size_t read_pos{0};
  void empty_uart_buffer();
  uint8_t check_incoming_crc_();
  uint16_t pipsolar_crc_(uint8_t *msg, uint8_t len);
};

}
}
