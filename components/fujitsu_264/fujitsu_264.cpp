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

  ESP_LOGD(TAG, "transmit_state() called");
  ESP_LOGD(TAG, "mode: %d, fan_mode: %d, swing_mode: %d, target_temp: %.1f", mode, fan_mode, swing_mode, target_temp);

  // Configure AC state
  if (mode == climate::CLIMATE_MODE_OFF) {
    ESP_LOGD(TAG, "Setting AC OFF");
    this->ac_.off();
  } else {
    ESP_LOGD(TAG, "Setting AC ON");
    this->ac_.on();

    // Set mode
    if (mode == climate::CLIMATE_MODE_AUTO) {
      ESP_LOGD(TAG, "Set mode: AUTO");
      this->ac_.setMode(kFujitsuAc264ModeAuto);
    } else if (mode == climate::CLIMATE_MODE_HEAT) {
      ESP_LOGD(TAG, "Set mode: HEAT");
      this->ac_.setMode(kFujitsuAc264ModeHeat);
    } else if (mode == climate::CLIMATE_MODE_COOL) {
      ESP_LOGD(TAG, "Set mode: COOL");
      this->ac_.setMode(kFujitsuAc264ModeCool);
    } else if (mode == climate::CLIMATE_MODE_DRY) {
      ESP_LOGD(TAG, "Set mode: DRY");
      this->ac_.setMode(kFujitsuAc264ModeDry);
    } else if (mode == climate::CLIMATE_MODE_FAN_ONLY) {
      ESP_LOGD(TAG, "Set mode: FAN_ONLY");
      this->ac_.setMode(kFujitsuAc264ModeFan);
    } else {
      ESP_LOGW(TAG, "Unsupported climate mode: %d", mode);
      return;
    }

    // Set temperature
    ESP_LOGD(TAG, "Set temperature: %.1f", target_temp);
    this->ac_.setTemp(target_temp);

    // Set fan mode
    if (fan_mode == climate::CLIMATE_FAN_AUTO) {
      ESP_LOGD(TAG, "Set fan: AUTO");
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedAuto);
    } else if (fan_mode == climate::CLIMATE_FAN_LOW) {
      ESP_LOGD(TAG, "Set fan: LOW");
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedLow);
    } else if (fan_mode == climate::CLIMATE_FAN_MEDIUM) {
      ESP_LOGD(TAG, "Set fan: MEDIUM");
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedMed);
    } else if (fan_mode == climate::CLIMATE_FAN_HIGH) {
      ESP_LOGD(TAG, "Set fan: HIGH");
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedHigh);
    } else if (fan_mode == climate::CLIMATE_FAN_QUIET) {
      ESP_LOGD(TAG, "Set fan: QUIET");
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedQuiet);
    } else {
      ESP_LOGW(TAG, "Unsupported fan mode: %d", fan_mode);
      this->ac_.setFanSpeed(kFujitsuAc264FanSpeedAuto);
    }

    // Set swing mode
    ESP_LOGD(TAG, "Set swing: %s", swing_mode == climate::CLIMATE_SWING_VERTICAL ? "VERTICAL" : "OFF");
    this->ac_.setSwing(swing_mode == climate::CLIMATE_SWING_VERTICAL);
  }

  auto state = this->ac_.getRaw();
  auto nbytes = this->ac_.getStateLength();

  ESP_LOGD(TAG, "Raw state length: %d", nbytes);
  ESP_LOGD(TAG, "Raw state bytes:");
  for (uint16_t i = 0; i < nbytes; i++) {
    ESP_LOGD(TAG, "  state[%d] = 0x%02X", i, state[i]);
  }

  auto transmit = this->transmitter_->transmit();
  auto *call_data = transmit.get_data();
  call_data->set_carrier_frequency(38000);  // 38kHz

  ESP_LOGD(TAG, "Sending IR header: mark %u, space %u", kFujitsuAcHdrMark, kFujitsuAcHdrSpace);
  call_data->mark(kFujitsuAcHdrMark);
  call_data->space(kFujitsuAcHdrSpace);

  for (uint16_t i = 0; i < nbytes; i++) {
    for (int8_t j = 7; j >= 0; j--) {
      call_data->mark(kFujitsuAcBitMark);
      bool bit = state[i] & (1 << j);
      if (bit) {
        call_data->space(kFujitsuAcOneSpace);
      } else {
        call_data->space(kFujitsuAcZeroSpace);
      }
      ESP_LOGVV(TAG, "Bit: %d (byte %d, bit %d) -> %s", bit, i, j, bit ? "ONE" : "ZERO");
    }
  }

  ESP_LOGD(TAG, "Sending IR footer: mark %u, space %u", kFujitsuAcBitMark, kFujitsuAcMinGap);
  call_data->mark(kFujitsuAcBitMark);
  call_data->space(kFujitsuAcMinGap);

  ESP_LOGD(TAG, "Performing IR transmit");
  transmit.perform();

  ESP_LOGD(TAG, "Fujitsu AC remote state sent");
}

}  // namespace fujitsu_264
}  // namespace esphome
