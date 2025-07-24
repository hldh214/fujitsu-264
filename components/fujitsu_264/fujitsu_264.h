#pragma once

#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"
#include "esphome/components/remote_transmitter/remote_transmitter.h"

#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "ir_Fujitsu.h"

namespace esphome {
    namespace ir_remote_base
    {
        class IrRemoteBase
        {
        protected:
            void sendGeneric(
                remote_base::RemoteTransmitterBase *transmitter_,
                const uint16_t headermark, const uint32_t headerspace,
                const uint16_t onemark, const uint32_t onespace,
                const uint16_t zeromark, const uint32_t zerospace,
                const uint16_t footermark, const uint32_t gap,
                const uint8_t *message, const uint16_t length,
                const uint16_t frequency)
            {
                auto transmit = transmitter_->transmit();
                auto *data = transmit.get_data();

                data->set_carrier_frequency(frequency);

                // Header
                if (headermark)
                    data->mark(headermark);
                else if (headerspace)
                    data->mark(1);
                if (headerspace)
                    data->space(headerspace);

                // Data
                for (uint8_t i = 0; i < length; i++)
                {
                    uint8_t d = *(message + i);
                    for (uint8_t bit = 0; bit < 8; bit++, d >>= 1)
                    {
                        if (d & 1)
                        {
                            data->mark(onemark);
                            data->space(onespace);
                        }
                        else
                        {
                            data->mark(zeromark);
                            data->space(zerospace);
                        }
                    }
                }

                // Footer
                if (footermark)
                    data->mark(footermark);
                else if (gap)
                    data->mark(1);
                if (gap)
                    data->space(gap);

                transmit.perform();
            }
        };
    } // namespace ir_remote_base

    namespace fujitsu_264
    {
        class Fujitsu264Climate : public climate_ir::ClimateIR, public ir_remote_base::IrRemoteBase
        {
        public:
            Fujitsu264Climate()
                : ClimateIR(kFujitsuAc264MinTemp, kFujitsuAc264MaxTemp, 1.0f, /* supports_dry */ true, /* supports_fan_only */ true,
                {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                       climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_QUIET},
                      {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL, climate::CLIMATE_SWING_BOTH}) {}
            void setup() override;

        protected:
            void transmit_state() override;

        private:
            void send();
            void apply_state();

            IRFujitsuAC264 ac_ = IRFujitsuAC264(255); // pin is not used
        };
    }  // namespace fujitsu_264
}  // namespace esphome
