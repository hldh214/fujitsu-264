import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID

AUTO_LOAD = ["climate_ir"]

fujitsu_264_ns = cg.esphome_ns.namespace("fujitsu_264")
Fujitsu264Climate = fujitsu_264_ns.class_(
    "Fujitsu264Climate", climate_ir.ClimateIR
)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(Fujitsu264Climate)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate_ir.register_climate_ir(var, config)
    cg.add_library(
        name="IRremoteESP8266",
        repository="https://github.com/shimmy-void/IRremoteESP8266.git#bce5c0b0eeda8431a95d95c9947e4008bb0b982f",
        version="bce5c0b0eeda8431a95d95c9947e4008bb0b982f",
    )
