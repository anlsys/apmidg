/*
  placeholder for copyright

  Developed by Kazutomo Yoshii <kazutomo@mcs.anl.gov>
 */

/**
 * @file libapmidg.h
 * @author Kazutomo Yoshii <kazutomo@mcs.anl.gov>
 * @date Jan 12, 2023
 * @brief A simple but sufficient C API for Intel discrete GPUs power management
 *
 * While the apmidig library is implemented in C++, C API is
 * convenient or required for many situations. The native C++ API
 * offers more flexibility than the functionality defined in this C
 * header.
 */

#ifndef __LIBIDGPUPOWER_H_DEFINED__
#define __LIBIDGPUPOWER_H_DEFINED__

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#include <stdint.h>


/**
 * @brief Initializes the power management functionality for Intel
 * discrete GPUs
 * @param[in] verbose
 * @return    return 0 if successful
 */
EXTERNC int  apmidg_init(int verbose); // return 0 if successful

/**
 * @brief Finalizes the power management.
 */
EXTERNC void apmidg_finish();


/**
 * @brief Returns the number of available devices (or GPUs). The
 * device id starts with zero.
 */
EXTERNC int  apmidg_getndevs();

// power domains

/**
 * @brief Returns the number of available power domains in the device
 * specified by 'devid'.
 */
EXTERNC int apmidg_getnpwrdoms(int devid);

/**
 * @brief Returns various properties on the power domain specified by
 * 'pwrid' in the device specified by 'devid'
 * @param[out] onsubdev  indicates whether the power domain is on a subdevice.
 * @param[out] subdevid  the subdevice id (if on a subdevice)
 * @param[out] canctrl   indicates whether it can control the power capping
 * @param[out] deflim_mw the default power capping in milliwatt
 * @param[out] minlim_mw the minimum power capping in milliwatt
 * @param[out] maxlim_mw the maximum power capping in milliwatt
 */
EXTERNC void apmidg_getpwrprops(int devid, int pwrid, int *onsubdev,
				int *subdevid, int *canctrl, int *deflim_mw,
				int *minlim_mw, int *maxlim_mw);

/**
 * @brief Gets the sustainable power limit. The unit is milliwatt.
 */
EXTERNC void apmidg_getpwrlim(int devid, int pwrid, int *lim_mw);
/**
 * @brief Sets the sustained power limit. The unit is milliwatt.
 */
EXTERNC void apmidg_setpwrlim(int devid, int pwrid, int lim_mw);

/**
 * @brief Reads the energy counter and the time stamp. The unit is micro joule.
 */
EXTERNC void apmidg_readenergy(int devid, int pwrid,
			       uint64_t *enery_uj, uint64_t *ts_usec);

/**
 * @brief Reads the average power. The unit is watt.
 */
EXTERNC double apmidg_readpoweravg(int devid, int pwrid);

// frequency domains

/**
 * @brief Returns the number of available frequency domains.
 */
EXTERNC int apmidg_getnfreqdoms(int devid);

/**
 * @brief Returns various properties on the frequency domain specified by
 * 'pwrid' in the device specified by 'devid'.
 */
EXTERNC void apmidg_getfreqprops(int devid, int freqid, int *onsubdev,
				int *subdevid, int *canctrl,
				 double *min_MHz, double *max_MHz);

/**
 * @brief Gets the frequency max and min limits.
 */
EXTERNC void apmidg_getfreqlims(int devid, int freqid,
				double *min_MHz, double *max_MHz);


/**
 * @brief Sets the frequency max and min limits.
 */
EXTERNC void apmidg_setfreqlims(int devid, int freqid,
				double min_MHz, double max_MHz);

/**
 * @brief Reads the current actual frequency.
 */
EXTERNC void apmidg_readfreq(int devid, int freqid, double *actual_MHz);


// temperature sensors

/**
 * @brief Returns the number of available temperature sensors.
 */
EXTERNC int apmidg_getntempsensors(int devid);

/**
 * @brief Returns various properties on the temperature sensors
 * specified by 'pwrid' in the device specified by 'devid'.
 */
EXTERNC void apmidg_gettempprops(int devid, int tempid, int *onsubdev,
				 int *subdevid,  int *type);

/**
 * @brief Returns the name string of specified sensor type
 */
EXTERNC const char* apmidg_sensortype_str(int type);

/**
 * @brief Reads the current temperature sensor value.
 */
EXTERNC void apmidg_readtemp(int devid, int tempid, double *temp_C);

#endif
