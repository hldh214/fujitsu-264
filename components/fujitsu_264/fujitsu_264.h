#include "esphome.h"
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "ir_Fujitsu.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace fujitsu_264 {
const uint16_t kIrLed = 32;  // for Living Room
IRFujitsuAC264 ac(kIrLed);

class Fujitsu264Climate : public climate::Climate
{
public:
  // sensor::Sensor *temp_sensor{nullptr};

  // void set_temp_sensor(sensor::Sensor *sensor) { this->temp_sensor = sensor; }

  void setup() // removed 'override'
  {
    ac.begin();
    ac.setMode(kFujitsuAc264ModeCool);
    ac.setTemp(25);
    ac.setFanSpeed(kFujitsuAc264FanSpeedAuto);
    ac.setSwing(true);  // true = on, false = off
    ac.off();

    this->mode = climate::CLIMATE_MODE_OFF;
    this->fan_mode = climate::CLIMATE_FAN_AUTO;
    this->swing_mode = climate::CLIMATE_SWING_BOTH;
    this->target_temperature = 25;
    this->publish_state();

    // if(this->temp_sensor != nullptr){
      // this->temp_sensor->add_on_raw_state_callback([this](float temp) { update_temp(temp); });
    // }
  }

  void update_temp(float temp)
  {
    if(isnan(temp)){
      return;
    }

    this->current_temperature = temp;
    this->publish_state();
  }

  climate::ClimateTraits traits()
  {
    auto traits = climate::ClimateTraits();

    // traits.set_supports_current_temperature(this->temp_sensor != nullptr);
    traits.set_supports_two_point_target_temperature(false);
    traits.set_visual_min_temperature(kFujitsuAc264MinTemp);  // 18C
    traits.set_visual_max_temperature(kFujitsuAc264MaxTemp);  // 30C
    traits.set_visual_temperature_step(0.5);

    std::set<climate::ClimateMode> climateModes;
    climateModes.insert(climate::CLIMATE_MODE_OFF);
    climateModes.insert(climate::CLIMATE_MODE_HEAT_COOL);
    climateModes.insert(climate::CLIMATE_MODE_COOL);
    climateModes.insert(climate::CLIMATE_MODE_HEAT);
    climateModes.insert(climate::CLIMATE_MODE_DRY);
    climateModes.insert(climate::CLIMATE_MODE_FAN_ONLY);

    traits.set_supported_modes(climateModes);

    std::set<climate::ClimateFanMode> climateFanModes;
    climateFanModes.insert(climate::CLIMATE_FAN_AUTO);
    climateFanModes.insert(climate::CLIMATE_FAN_QUIET);
    climateFanModes.insert(climate::CLIMATE_FAN_LOW);
    climateFanModes.insert(climate::CLIMATE_FAN_MEDIUM);
    climateFanModes.insert(climate::CLIMATE_FAN_HIGH);

    traits.set_supported_fan_modes(climateFanModes);

    std::set<climate::ClimateSwingMode> climateSwingModes;
    climateSwingModes.insert(climate::CLIMATE_SWING_BOTH);
    climateSwingModes.insert(climate::CLIMATE_SWING_VERTICAL);
    climateSwingModes.insert(climate::CLIMATE_SWING_HORIZONTAL);

    traits.set_supported_swing_modes(climateSwingModes);

    return traits;
  }

  void control(const climate::ClimateCall &call) override
  {
    if (call.get_mode().has_value())
    {
      climate::ClimateMode climateMode = *call.get_mode();
      switch (climateMode)
      {
      case climate::CLIMATE_MODE_HEAT:
        ac.setMode(kFujitsuAc264ModeHeat);
        ac.on();
        break;
      case climate::CLIMATE_MODE_COOL:
        ac.setMode(kFujitsuAc264ModeCool);
        ac.on();
        break;
      case climate::CLIMATE_MODE_HEAT_COOL:
        ac.setMode(kFujitsuAc264ModeAuto);
        ac.on();
        break;
      case climate::CLIMATE_MODE_DRY:
        ac.setMode(kFujitsuAc264ModeDry);
        ac.on();
        break;
      case climate::CLIMATE_MODE_FAN_ONLY:
        ac.setMode(kFujitsuAc264ModeFan);
        ac.on();
        break;
      case climate::CLIMATE_MODE_OFF:
        ac.off();
        break;
      }

      this->mode = climateMode;
      this->publish_state();
    }

    if (call.get_fan_mode().has_value())
    {
      climate::ClimateFanMode fanMode = *call.get_fan_mode();
      switch (fanMode)
      {
      case climate::CLIMATE_FAN_AUTO:
        ac.setFanSpeed(kFujitsuAc264FanSpeedAuto);
        break;
      case climate::CLIMATE_FAN_QUIET:
        ac.setFanSpeed(kFujitsuAc264FanSpeedQuiet);
        break;
      case climate::CLIMATE_FAN_LOW:
        ac.setFanSpeed(kFujitsuAc264FanSpeedLow);
        break;
      case climate::CLIMATE_FAN_MEDIUM:
        ac.setFanSpeed(kFujitsuAc264FanSpeedMed);
        break;
      case climate::CLIMATE_FAN_HIGH:
        ac.setFanSpeed(kFujitsuAc264FanSpeedHigh);
        break;
      }

      this->fan_mode = fanMode;
      this->publish_state();
    }

    if (call.get_swing_mode().has_value())
    {
      climate::ClimateSwingMode swingMode = *call.get_swing_mode();
      switch (swingMode)
      {
      case climate::CLIMATE_SWING_BOTH:
        ac.setSwing(true);
        break;
      case climate::CLIMATE_SWING_VERTICAL:
        ac.setSwing(false);
        ac.setFanAngle(kFujitsuAc264FanAngle1);
        break;
      case climate::CLIMATE_SWING_HORIZONTAL:
        ac.setSwing(false);
        ac.setFanAngle(kFujitsuAc264FanAngle7);
        break;
      }

      this->swing_mode = swingMode;
      this->publish_state();
    }

    if (call.get_target_temperature().has_value())
    {
      float temp = *call.get_target_temperature();
      ac.setTemp(temp);

      this->target_temperature = temp;
      this->publish_state();
    }

    ac.send();
  }
};

}}