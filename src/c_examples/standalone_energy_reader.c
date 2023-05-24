/*
  Standalone code to read the gpu energy through sysman

  Note: by default, it targets driver id 0.

  (setq c-basic-offset 2)
*/

#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define CALL_ZEINIT (1)

////////////////////////////////////////////


#define _ZE_ERROR_MSG(NAME,RES) {fprintf(stderr,"%s() failed at %d(%s): res=%x\n",(NAME),__LINE__,__FILE__,(RES));}
#define _ZE_ERROR_MSG_NOTERMINATE(NAME,RES) {fprintf(stderr,"%s() error at %d(%s): res=%x\n",(NAME),__LINE__,__FILE__,(RES));}
#define _ERROR_MSG(MSG) {perror((MSG)); fprintf(stderr,"errno=%d at %d(%s)",errno,__LINE__,__FILE__);}

static int initialized = 0;

#define MAX_N_DEVS  (8)
#define MAX_N_PDOMS (8)

static int selected_driver_id = 0;
static ze_driver_handle_t selected_drvh;

static uint32_t n_devhs;
static ze_device_handle_t  devhs_cache[MAX_N_DEVS];
static zes_pwr_handle_t   mainpwrh_per_dev[MAX_N_DEVS]; // main power domain associated with each device
static int canreadpwrh_per_dev[MAX_N_DEVS];


int zerGetNDevs()
{
  if(!initialized) return 0;
  return n_devhs;
}

void  zerReadEnergy(int devid, uint64_t *ts_us, uint64_t *energy_uj)
{
  *ts_us = 0;
  *energy_uj = 0;
  if (!initialized) return;

  if (canreadpwrh_per_dev[devid]) {
    zes_power_energy_counter_t ecounter;
    zesPowerGetEnergyCounter(mainpwrh_per_dev[devid], &ecounter);
    *ts_us = ecounter.timestamp;
    *energy_uj = ecounter.energy;
  }
}

// return non-zero if failed to initialize
int zerInit()
{
  ze_result_t res;

  const char *e = getenv("ZES_ENABLE_SYSMAN");
  if (!(e && e[0] == '1'))  {
	fprintf(stderr,"ZES_ENABLE_SYSMAN needs to be set!\n");
	return -1;
  }

#ifdef CALL_ZEINIT
  res = zeInit(ZE_INIT_FLAG_GPU_ONLY);
  if (res != ZE_RESULT_SUCCESS) {
    _ZE_ERROR_MSG("zeInit", res);
    return -1;
  }
#endif

  /*
   * detect drivers
   */
  uint32_t n_drvs = 0;
  res = zeDriverGet(&n_drvs, NULL);
  if (res != ZE_RESULT_SUCCESS || n_drvs == 0) {
	fprintf(stderr, "ERROR: No driver found!\n");
	_ZE_ERROR_MSG("zeDriverGet", res);
	return -1;
  }
  ze_driver_handle_t *drvhs = (ze_driver_handle_t*)alloca(n_drvs*sizeof(ze_driver_handle_t));
  printf("n_drvs=%d\n", n_drvs);

  res = zeDriverGet(&n_drvs, drvhs);
  if (res != ZE_RESULT_SUCCESS) {
    _ZE_ERROR_MSG("2nd zeDriverGet", res);
    return -1;
  }

  selected_drvh = drvhs[selected_driver_id]; // assume this is a deep-copy

  /*
   * detect devices on selected_drvh
   */
  n_devhs = 0;
  res = zeDeviceGet(selected_drvh, &n_devhs, NULL);
  if (res != ZE_RESULT_SUCCESS || n_devhs == 0) {
	fprintf(stderr, "ERROR: No device found!\n");
	_ZE_ERROR_MSG("zeDeviceGet", res);
	return -1;
  }
  if (n_devhs >= MAX_N_DEVS) {
	fprintf(stderr, "ERROR: %d devs are detected, which exceeds MAX_N_DEVS\n", n_devhs);
	return -1;
  }

  res = zeDeviceGet(selected_drvh, &n_devhs, devhs_cache);
  if (res != ZE_RESULT_SUCCESS) {
	_ZE_ERROR_MSG("2nd zeDeviceGet", res);
	return -1;
  }

  // iterate devices to find power domains associated with each device
  for (int i=0; i<n_devhs; i++) {
    canreadpwrh_per_dev[i] = 0;

    ze_device_properties_t props = {0};
    props.stype = ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES;
    props.pNext = NULL;
    res = zeDeviceGetProperties(devhs_cache[i], &props);
    if (res != ZE_RESULT_SUCCESS) {
      _ZE_ERROR_MSG("zeDeviceGetProperties", res);
      return -1;
    }
    if (props.type == ZE_DEVICE_TYPE_GPU) {
      zes_device_handle_t smh = smh = (zes_device_handle_t)devhs_cache[i];
      zes_pwr_handle_t pwrhs[MAX_N_PDOMS];
      uint32_t npwrdoms = 0;

      res = zesDeviceEnumPowerDomains(smh, &npwrdoms, NULL);
      if (res != ZE_RESULT_SUCCESS) {
	_ZE_ERROR_MSG("zesDeviceEnumPowerDomains", res);
      } else {
	if (npwrdoms > 0 && npwrdoms <= MAX_N_PDOMS) {
	  res = zesDeviceEnumPowerDomains(smh, &npwrdoms, pwrhs);
	  if (res != ZE_RESULT_SUCCESS) {
	    _ZE_ERROR_MSG("zesDeviceEnumPowerDomains", res);
	    return -1;
					   }
	  mainpwrh_per_dev[i] = pwrhs[0];
	  canreadpwrh_per_dev[i] = 1;
	  if(0) {
	    zes_power_energy_counter_t ecounter;
	    res = zesPowerGetEnergyCounter(mainpwrh_per_dev[i], &ecounter);
	    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetEnergyCounter", res);
	    printf("%lu us  %lu uj\n", ecounter.timestamp, ecounter.energy);
	  }
	} else {
	  fprintf(stderr, "Warning: npwrdoms=%d\n", npwrdoms);
	}
      }
    } else {
      fprintf(stderr, "Warning: dev%d is not a GPU!\n", i);
    }
  }

  initialized = 1;
  return 0;
}


/////////////////////////////////

int main(int argc, char *argv[])
{
  uint64_t ts_us;
  uint64_t energy_uj;

  zerInit();

  for (int i=0; i<3; i++) {
    for  (int j=0; j<zerGetNDevs(); j++) {
      zerReadEnergy(j, &ts_us, &energy_uj);
      printf("dev%d ts_us=%lu energy_uj=%lu\n", j, ts_us, energy_uj);
    }
    sleep(1);
  }

  return 0;
}
