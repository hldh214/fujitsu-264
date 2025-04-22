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
  climate_ir::ClimateIR::setup();
  ac.begin();
  ac.setMode(kFujitsuAc264ModeCool);
  ac.setTemp(25);
  ac.setFanSpeed(kFujitsuAc264FanSpeedAuto);
  ac.setSwing(true);  // true = on, false = off
  ac.off();
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

  ESP_LOGD(TAG, "transmit_state() called");
  ESP_LOGD(TAG, "mode: %d, fan_mode: %d, swing_mode: %d, target_temp: %.1f", mode, fan_mode, swing_mode, target_temp);

  // Configure AC state
  if (mode == climate::CLIMATE_MODE_OFF) {
    ESP_LOGD(TAG, "Setting AC OFF");
    ac.off();
  } else {
    ESP_LOGD(TAG, "Setting AC ON");
    ac.on();

    // Set mode
    if (mode == climate::CLIMATE_MODE_AUTO) {
      ESP_LOGD(TAG, "Set mode: AUTO");
      ac.setMode(kFujitsuAc264ModeAuto);
    } else if (mode == climate::CLIMATE_MODE_HEAT) {
      ESP_LOGD(TAG, "Set mode: HEAT");
      ac.setMode(kFujitsuAc264ModeHeat);
    } else if (mode == climate::CLIMATE_MODE_COOL) {
      ESP_LOGD(TAG, "Set mode: COOL");
      ac.setMode(kFujitsuAc264ModeCool);
    } else if (mode == climate::CLIMATE_MODE_DRY) {
      ESP_LOGD(TAG, "Set mode: DRY");
      ac.setMode(kFujitsuAc264ModeDry);
    } else if (mode == climate::CLIMATE_MODE_FAN_ONLY) {
      ESP_LOGD(TAG, "Set mode: FAN_ONLY");
      ac.setMode(kFujitsuAc264ModeFan);
    } else {
      ESP_LOGW(TAG, "Unsupported climate mode: %d", mode);
      return;
    }

    // Set temperature
    ESP_LOGD(TAG, "Set temperature: %.1f", target_temp);
    ac.setTemp(target_temp);

    // Set fan mode
    if (fan_mode == climate::CLIMATE_FAN_AUTO) {
      ESP_LOGD(TAG, "Set fan: AUTO");
      ac.setFanSpeed(kFujitsuAc264FanSpeedAuto);
    } else if (fan_mode == climate::CLIMATE_FAN_LOW) {
      ESP_LOGD(TAG, "Set fan: LOW");
      ac.setFanSpeed(kFujitsuAc264FanSpeedLow);
    } else if (fan_mode == climate::CLIMATE_FAN_MEDIUM) {
      ESP_LOGD(TAG, "Set fan: MEDIUM");
      ac.setFanSpeed(kFujitsuAc264FanSpeedMed);
    } else if (fan_mode == climate::CLIMATE_FAN_HIGH) {
      ESP_LOGD(TAG, "Set fan: HIGH");
      ac.setFanSpeed(kFujitsuAc264FanSpeedHigh);
    } else if (fan_mode == climate::CLIMATE_FAN_QUIET) {
      ESP_LOGD(TAG, "Set fan: QUIET");
      ac.setFanSpeed(kFujitsuAc264FanSpeedQuiet);
    } else {
      ESP_LOGW(TAG, "Unsupported fan mode: %d", fan_mode);
      ac.setFanSpeed(kFujitsuAc264FanSpeedAuto);
    }

    // Set swing mode
    ESP_LOGD(TAG, "Set swing: %s", swing_mode == climate::CLIMATE_SWING_VERTICAL ? "VERTICAL" : "OFF");
    ac.setSwing(swing_mode == climate::CLIMATE_SWING_VERTICAL);
  }

  ac.send();
  ESP_LOGD(TAG, "Fujitsu AC remote state sent");
}

}  // namespace fujitsu_264
}  // namespace esphome
