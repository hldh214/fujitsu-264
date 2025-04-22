#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate_ir/climate_ir.h"
#include <IRremoteESP8266.h>
#include <ir_Fujitsu.h>

namespace esphome {
namespace fujitsu_264 {

const uint16_t FUJITSU_IR_LED = 32;  // FIXME: Can we make this configurable?

class Fujitsu264Climate : public climate_ir::ClimateIR {
 public:
  Fujitsu264Climate()
  : climate_ir::ClimateIR(kFujitsuAc264MinTemp, kFujitsuAc264MaxTemp, 1.0f),
  ac (FUJITSU_IR_LED) {}

  void setup() override;
  climate::ClimateTraits traits() override;

 protected:
  void transmit_state() override;

 private:
  IRFujitsuAC264 ac;
};

}  // namespace fujitsu_264
}  // namespace esphome
