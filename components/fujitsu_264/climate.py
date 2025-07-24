import esphome.codegen as cg
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]

fujitsu_264_ns = cg.esphome_ns.namespace("fujitsu_264")
Fujitsu264Climate = fujitsu_264_ns.class_(
    "Fujitsu264Climate", climate_ir.ClimateIR
)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(Fujitsu264Climate)


async def to_code(config):
    cg.add_library(
        name="IRremoteESP8266",
        repository="https://github.com/hldh214/IRremoteESP8266.git#564c20fa2e345420598bd0286d7070036551928b",
        version="564c20fa2e345420598bd0286d7070036551928b",
    )

    await climate_ir.new_climate_ir(config)
