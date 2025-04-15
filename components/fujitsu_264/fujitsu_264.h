#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate_ir/climate_ir.h"
#include <IRremoteESP8266.h>
#include <ir_Fujitsu.h>

namespace esphome {
namespace fujitsu_264 {

class Fujitsu264Climate : public climate_ir::ClimateIR {
 public:
  Fujitsu264Climate() : climate_ir::ClimateIR(kFujitsuAc264MinTemp, kFujitsuAc264MaxTemp, 1.0) {}
  
  void setup() override;
  climate::ClimateTraits traits() override;
  
 protected:
  void transmit_state() override;
                
 private:
  IRFujitsuAC264 ac_{0};
};

}  // namespace fujitsu_264
}  // namespace esphome
