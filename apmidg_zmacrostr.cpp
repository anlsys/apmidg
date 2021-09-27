/*
  placeholder for copyright

  Written by Kazutomo Yoshii <kazutomo@mcs.anl.gov>
 */

#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>
#include <iostream>

struct _ze_code_string {
    int no;
    const char *str;
};

#define CONV(ZZ) #ZZ

static struct _ze_code_string  ze_err_code_str[] = {
    {ZE_RESULT_SUCCESS, CONV(ZE_RESULT_SUCCESS)},
    {ZE_RESULT_NOT_READY, CONV(ZE_RESULT_NOT_READY)},
    {ZE_RESULT_ERROR_DEVICE_LOST, CONV(ZE_RESULT_ERROR_DEVICE_LOST)},
    {ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY, CONV(ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY)},
    {ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY, CONV(ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY)},
    {ZE_RESULT_ERROR_MODULE_BUILD_FAILURE, CONV(ZE_RESULT_ERROR_MODULE_BUILD_FAILURE)},
    {ZE_RESULT_ERROR_MODULE_LINK_FAILURE, CONV(ZE_RESULT_ERROR_MODULE_LINK_FAILURE)},
    {ZE_RESULT_ERROR_DEVICE_REQUIRES_RESET, CONV(ZE_RESULT_ERROR_DEVICE_REQUIRES_RESET)},
    {ZE_RESULT_ERROR_DEVICE_IN_LOW_POWER_STATE, CONV(ZE_RESULT_ERROR_DEVICE_IN_LOW_POWER_STATE)},
    {ZE_RESULT_ERROR_INSUFFICIENT_PERMISSIONS, CONV(ZE_RESULT_ERROR_INSUFFICIENT_PERMISSIONS)},
    {ZE_RESULT_ERROR_NOT_AVAILABLE, CONV(ZE_RESULT_ERROR_NOT_AVAILABLE)},
    {ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE, CONV(ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE)},
    {ZE_RESULT_ERROR_UNINITIALIZED, CONV(ZE_RESULT_ERROR_UNINITIALIZED)},
    {ZE_RESULT_ERROR_UNSUPPORTED_VERSION, CONV(ZE_RESULT_ERROR_UNSUPPORTED_VERSION)},
    {ZE_RESULT_ERROR_UNSUPPORTED_FEATURE, CONV(ZE_RESULT_ERROR_UNSUPPORTED_FEATURE)},
    {ZE_RESULT_ERROR_INVALID_ARGUMENT, CONV(ZE_RESULT_ERROR_INVALID_ARGUMENT)},
    {ZE_RESULT_ERROR_INVALID_NULL_HANDLE, CONV(ZE_RESULT_ERROR_INVALID_NULL_HANDLE)},
    {ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE, CONV(ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE)},
    {ZE_RESULT_ERROR_INVALID_NULL_POINTER, CONV(ZE_RESULT_ERROR_INVALID_NULL_POINTER)},
    {ZE_RESULT_ERROR_INVALID_SIZE, CONV(ZE_RESULT_ERROR_INVALID_SIZE)},
    {ZE_RESULT_ERROR_UNSUPPORTED_SIZE, CONV(ZE_RESULT_ERROR_UNSUPPORTED_SIZE)},
    {ZE_RESULT_ERROR_UNSUPPORTED_ALIGNMENT, CONV(ZE_RESULT_ERROR_UNSUPPORTED_ALIGNMENT)},
    {ZE_RESULT_ERROR_INVALID_SYNCHRONIZATION_OBJECT, CONV(ZE_RESULT_ERROR_INVALID_SYNCHRONIZATION_OBJECT)},
    {ZE_RESULT_ERROR_INVALID_ENUMERATION, CONV(ZE_RESULT_ERROR_INVALID_ENUMERATION)},
    {ZE_RESULT_ERROR_UNSUPPORTED_ENUMERATION, CONV(ZE_RESULT_ERROR_UNSUPPORTED_ENUMERATION)},
    {ZE_RESULT_ERROR_UNSUPPORTED_IMAGE_FORMAT, CONV(ZE_RESULT_ERROR_UNSUPPORTED_IMAGE_FORMAT)},
    {ZE_RESULT_ERROR_INVALID_NATIVE_BINARY, CONV(ZE_RESULT_ERROR_INVALID_NATIVE_BINARY)},
    {ZE_RESULT_ERROR_INVALID_GLOBAL_NAME, CONV(ZE_RESULT_ERROR_INVALID_GLOBAL_NAME)},
    {ZE_RESULT_ERROR_INVALID_KERNEL_NAME, CONV(ZE_RESULT_ERROR_INVALID_KERNEL_NAME)},
    {ZE_RESULT_ERROR_INVALID_FUNCTION_NAME, CONV(ZE_RESULT_ERROR_INVALID_FUNCTION_NAME)},
    {ZE_RESULT_ERROR_INVALID_GROUP_SIZE_DIMENSION, CONV(ZE_RESULT_ERROR_INVALID_GROUP_SIZE_DIMENSION)},
    {ZE_RESULT_ERROR_INVALID_GLOBAL_WIDTH_DIMENSION, CONV(ZE_RESULT_ERROR_INVALID_GLOBAL_WIDTH_DIMENSION)},
    {ZE_RESULT_ERROR_INVALID_KERNEL_ARGUMENT_INDEX, CONV(ZE_RESULT_ERROR_INVALID_KERNEL_ARGUMENT_INDEX)},
    {ZE_RESULT_ERROR_INVALID_KERNEL_ARGUMENT_SIZE, CONV(ZE_RESULT_ERROR_INVALID_KERNEL_ARGUMENT_SIZE)},
    {ZE_RESULT_ERROR_INVALID_KERNEL_ATTRIBUTE_VALUE, CONV(ZE_RESULT_ERROR_INVALID_KERNEL_ATTRIBUTE_VALUE)},
    {ZE_RESULT_ERROR_INVALID_MODULE_UNLINKED, CONV(ZE_RESULT_ERROR_INVALID_MODULE_UNLINKED)},
    {ZE_RESULT_ERROR_INVALID_COMMAND_LIST_TYPE, CONV(ZE_RESULT_ERROR_INVALID_COMMAND_LIST_TYPE)},
    {ZE_RESULT_ERROR_OVERLAPPING_REGIONS, CONV(ZE_RESULT_ERROR_OVERLAPPING_REGIONS)}
};

static struct _ze_code_string  zes_temp_sensor_str[] = {
    {ZES_TEMP_SENSORS_GLOBAL, CONV(ZES_TEMP_SENSORS_GLOBAL)},
    {ZES_TEMP_SENSORS_GPU, CONV(ZES_TEMP_SENSORS_GPU)},
    {ZES_TEMP_SENSORS_MEMORY, CONV(ZES_TEMP_SENSORS_MEMORY)},
    {ZES_TEMP_SENSORS_GLOBAL_MIN, CONV(ZES_TEMP_SENSORS_GLOBAL_MIN)},
    {ZES_TEMP_SENSORS_GPU_MIN, CONV(ZES_TEMP_SENSORS_GPU_MIN)},
    {ZES_TEMP_SENSORS_MEMORY_MIN, CONV(ZES_TEMP_SENSORS_MEMORY_MIN)}
};


const char *str_ze_result_t(int code)
{
    int n = sizeof(ze_err_code_str)/sizeof(struct _ze_code_string);
    int i;

    for (i=0; i<n; i++) {
	if (ze_err_code_str[i].no == code) return ze_err_code_str[i].str;
    }
    return "UNKNOWN";
}

const char *str_zes_temp_sensors_t(int code)
{
    int n = sizeof(zes_temp_sensor_str)/sizeof(struct _ze_code_string);
    int i;

    for (i=0; i<n; i++) {
	if (zes_temp_sensor_str[i].no == code) return zes_temp_sensor_str[i].str;
    }
    return "UNKNOWN";
}

#if 0
int main()
{
    std::cout << str_ze_result_t(ZE_RESULT_ERROR_OVERLAPPING_REGIONS) << std::endl;
    std::cout << str_ze_result_t(ZE_RESULT_ERROR_OVERLAPPING_REGIONS+1) << std::endl;

    std::cout << str_zes_temp_sensors_t(ZES_TEMP_SENSORS_MEMORY_MIN) << std::endl;
    std::cout << str_zes_temp_sensors_t(ZES_TEMP_SENSORS_MEMORY_MIN+1) << std::endl;

    return 0;
}
#endif
