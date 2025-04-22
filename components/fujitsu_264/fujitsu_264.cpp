#include "fujitsu_264.h"

namespace esphome {
namespace fujitsu_264 {

static const char *const TAG = "fujitsu_264";

  void Fujitsu264Climate::setup () {
    climate_ir::ClimateIR::setup ();
    ac_.begin ();
    ac_.setMode (kFujitsuAc264ModeCool);
  }

void Fujitsu264Climate::transmit_state() {
  if (this->mode == climate::CLIMATE_MODE_OFF) {
    ac_.off ();
    ac_.send ();
    return;
  }

  ESP_LOGD(TAG, "Transmit state");
  ESP_LOGD(TAG, "Mode: %d", this->mode);
  ESP_LOGD(TAG, "Target temperature: %d", this->target_temperature);
  ESP_LOGD(TAG, "Fan mode: %d", this->fan_mode.value());
  ESP_LOGD(TAG, "Swing mode: %d", this->swing_mode);

  ac_.on ();
  ac_.setTemp (this->target_temperature);

  // Set mode
  switch (this->mode) {
    case climate::CLIMATE_MODE_COOL:
      ac_.setMode(kFujitsuAc264ModeCool);
      break;
    case climate::CLIMATE_MODE_HEAT:
      ac_.setMode(kFujitsuAc264ModeHeat);
      break;
    case climate::CLIMATE_MODE_DRY:
      ac_.setMode(kFujitsuAc264ModeDry);
      break;
    case climate::CLIMATE_MODE_FAN_ONLY:
      ac_.setMode(kFujitsuAc264ModeFan);
      break;
    case climate::CLIMATE_MODE_HEAT_COOL:
    default:
      ac_.setMode(kFujitsuAc264ModeAuto);
      break;
  }

  // Set fan
  switch (this->fan_mode.value()) {
    case climate::CLIMATE_FAN_HIGH:
      ac_.setFanSpeed(kFujitsuAc264FanSpeedHigh);
      break;
    case climate::CLIMATE_FAN_MEDIUM:
      ac_.setFanSpeed(kFujitsuAc264FanSpeedMed);
      break;
    case climate::CLIMATE_FAN_LOW:
      ac_.setFanSpeed(kFujitsuAc264FanSpeedLow);
      break;
    case climate::CLIMATE_FAN_QUIET:
      ac_.setFanSpeed(kFujitsuAc264FanSpeedQuiet);
      break;
    case climate::CLIMATE_FAN_AUTO:
    default:
      ac_.setFanSpeed(kFujitsuAc264FanSpeedAuto);
      break;
  }

  // Set swing
  switch (this->swing_mode) {
    case climate::CLIMATE_SWING_VERTICAL:
      ac_.setSwing(false);
      ac_.setFanAngle(kFujitsuAc264FanAngle1);
      break;
    case climate::CLIMATE_SWING_HORIZONTAL:
      ac_.setSwing(false);
      ac_.setFanAngle(kFujitsuAc264FanAngle7);
      break;
    case climate::CLIMATE_SWING_BOTH:
      ac_.setSwing(true);
      break;
    case climate::CLIMATE_SWING_OFF:
    default:
      ac_.setSwing(false);
      break;
  }

  ac_.send ();
}

}  // namespace fujitsu_264
}  // namespace esphome
