
# this script is to convert regular ze/zes API name to THAPI internal name. local use only

cat standalone_energy_reader.c | \
    sed s/zeDeviceGetProperties/ZE_DEVICE_GET_PROPERTIES_PTR/g | \
    sed s/zesDeviceEnumPowerDomains/ZES_DEVICE_ENUM_POWER_DOMAINS_PTR/g | \
    sed s/zesPowerGetEnergyCounter/ZES_POWER_GET_ENERGY_COUNTER_PTR/g | \
    sed s/zeDriverGet/ZE_DRIVER_GET_PTR/g | \
    sed s/zeDeviceGet/ZE_DEVICE_GET_PTR/g | \
    tee theapi_energy_ready.c
