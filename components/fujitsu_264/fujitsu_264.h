#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"

#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "ir_Fujitsu.h"

namespace esphome {
  namespace fujitsu_264 {

    const uint8_t FUJITSU_264_TEMP_MIN = 18;  // Celsius
    const uint8_t FUJITSU_264_TEMP_MAX = 30;  // Celsius
    const uint16_t FUJITSU_264_IR_LED = 32;  // for Living Room

    class Fujitsu264Climate : public climate_ir::ClimateIR {
      public:
        Fujitsu264Climate()
          : ClimateIR(FUJITSU_264_TEMP_MIN, FUJITSU_264_TEMP_MAX, 0.5f, /* supports_dry */ true, /* supports_fan_only */ true,
                      {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                       climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                      {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL, climate::CLIMATE_SWING_BOTH}),
            ac_(FUJITSU_264_IR_LED) {}

      protected:
        /// Transmit via IR the state of this climate controller.
        void transmit_state() override;
        void setup() override;
        IRFujitsuAC264 ac_;
    };

}  // namespace fujitsu_264
}  // namespace esphome
