#include "fujitsu_264.h"
#include "esphome/core/log.h"

// Ref:
// These values are based on averages of measurements
const uint16_t kFujitsuAcHdrMark = 3324;
const uint16_t kFujitsuAcHdrSpace = 1574;
const uint16_t kFujitsuAcBitMark = 448;
const uint16_t kFujitsuAcOneSpace = 1182;
const uint16_t kFujitsuAcZeroSpace = 390;
const uint16_t kFujitsuAcMinGap = 8100;

namespace esphome {
namespace fujitsu_264 {

static const char *const TAG = "fujitsu_264.climate";

void Fujitsu264Climate::setup() {
  this->ac_.begin();
  climate_ir::ClimateIR::setup();
}

climate::ClimateTraits Fujitsu264Climate::traits() {
  auto traits = climate_ir::ClimateIR::traits();

  traits.set_supported_modes({
    climate::CLIMATE_MODE_OFF,
    climate::CLIMATE_MODE_HEAT,
    climate::CLIMATE_MODE_COOL,
    climate::CLIMATE_MODE_DRY,
    climate::CLIMATE_MODE_FAN_ONLY,
    climate::CLIMATE_MODE_AUTO
  });

  traits.set_supported_fan_modes({
    climate::CLIMATE_FAN_AUTO,
    climate::CLIMATE_FAN_LOW,
    climate::CLIMATE_FAN_MEDIUM,
    climate::CLIMATE_FAN_HIGH,
    climate::CLIMATE_FAN_QUIET
  });

  traits.set_supported_swing_modes({
    climate::CLIMATE_SWING_OFF,
    climate::CLIMATE_SWING_VERTICAL
  });

  return traits;
}

void Fujitsu264Climate::transmit_state() {
  // Get current state
  auto mode = this->mode;
  auto fan_mode = this->fan_mode;
  auto swing_mode = this->swing_mode;
  auto target_temp = this->target_temperature;

  // Configure AC state
  if (mode == climate::CLIMATE_MODE_OFF) {
    this->ac_.off();
  } else {
    this->ac_.on();

    // Set mode
    if (mode == climate::CLIMATE_MODE_AUTO) {
      this->ac_.setMode(kFujitsuAc264ModeAuto);
    } else if (mode == climate::CLIMATE_MODE_HEAT) {
      this->ac_.setMode(kFujitsuAc264ModeHeat);
    } else if (mode == climate::CLIMATE_MODE_COOL) {
      this->ac_.setMode(kFujitsuAc264ModeCool);
    } else if (mode == climate::CLIMATE_MODE_DRY) {
      this->ac_.setMode(kFujitsuAc264ModeDry);
    } else if (mode == climate::CLIMATE_MODE_FAN_ONLY) {
      this->ac_.setMode(kFujitsuAc264ModeFan);
    } else {
      ESP_LOGW(TAG, "Unsupported climate mode: %d", static_cast<int>(mode));
      return;
    }

    // Set temperature
    this->ac_.setTemp(target_temp);

    // Set fan mode
    if (fan_mode == climate::CLIMATE_FAN_AUTO) {
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedAuto);
    } else if (fan_mode == climate::CLIMATE_FAN_LOW) {
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedLow);
    } else if (fan_mode == climate::CLIMATE_FAN_MEDIUM) {
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedMed);
    } else if (fan_mode == climate::CLIMATE_FAN_HIGH) {
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedHigh);
    } else if (fan_mode == climate::CLIMATE_FAN_QUIET) {
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedQuiet);
    } else {
      ESP_LOGW(TAG, "Unsupported fan mode: %d", static_cast<int>(fan_mode));
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedAuto);
    }

    // Set swing mode
    this->ac_.setSwing(swing_mode == climate::CLIMATE_SWING_VERTICAL);
  }

  auto state = this->ac_.getRaw();
  auto nbytes = this->ac_.getStateLength();

  auto transmit = this->transmitter_->transmit();
  auto *call_data = transmit.get_data();
  call_data->set_carrier_frequency(38000);  // 38kHz

  call_data->mark(kFujitsuAcHdrMark);
  call_data->space(kFujitsuAcHdrSpace);

  for (uint16_t i = 0; i < nbytes; i++) {
    for (int8_t j = 7; j >= 0; j--) {
      call_data->mark(kFujitsuAcBitMark);
      if (state[i] & (1 << j)) {
        call_data->space(kFujitsuAcOneSpace);
      } else {
        call_data->space(kFujitsuAcZeroSpace);
      }
    }
  }

  call_data->mark(kFujitsuAcBitMark);
  call_data->space(kFujitsuAcMinGap);

  transmit.perform();

  ESP_LOGD(TAG, "Fujitsu AC remote state sent");
}

}  // namespace fujitsu_264
}  // namespace esphome
