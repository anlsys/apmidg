/*
  Argo Power Management Glue Layer for Intel Discrete GPUs

  Level Zero APIs are described here
  https://spec.oneapi.com/level-zero/latest/index.html

  (setq c-basic-offset 4)
*/

#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>

#include "apmidg_zmacrostr.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <mutex>

#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define _ZE_ERROR_MSG(NAME,RES) {printf("%s() failed at %d(%s): res=%x:%s\n",(NAME),__LINE__,__FILE__,(RES),str_ze_result_t(RES)); std::terminate();}
#define _ZE_ERROR_MSG_NOTERMINATE(NAME,RES) {printf("%s() error at %d(%s): res=%x:%s\n",(NAME),__LINE__,__FILE__,(RES),str_ze_result_t(RES));}
#define _ERROR_MSG(MSG) {perror((MSG)); printf("errno=%d at %d(%s)",errno,__LINE__,__FILE__);}


class IDGPowerPerDevice {
    int verbose;
    int devid;
    ze_device_handle_t dev;
    zes_device_handle_t smh; // sysman handles
    std::vector<zes_pwr_handle_t> pwrhs;
    // prev_energy_uj and prev_ts_us are used to calculate poweravg_w
    // sampleenergy update these values
    std::vector<uint64_t> prev_energy_uj;
    std::vector<uint64_t> prev_ts_us;
    std::vector<double> poweravg_w;

    std::vector<zes_freq_handle_t> freqhs;
    std::vector<zes_temp_handle_t> temphs;

    // if a feature is unavailable for some reason, the following flags will be set.
    bool enabled_powerlimit;

    // assume these properties are static
    bool isgpu;
    uint32_t npwrdoms;
    uint32_t nfreqdoms;
    uint32_t ntempsensors;

public:
    IDGPowerPerDevice(ze_device_handle_t _dev, const int _devid, const int _ver = 1) {
	ze_result_t res;

	dev = _dev;
	devid = _devid;
	verbose = _ver;

	ze_device_properties_t devprop = {};

	res = zeDeviceGetProperties(dev, &devprop);
	if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zeDeviceGetProperties", res);
	if (devprop.type == ZE_DEVICE_TYPE_GPU) isgpu=true; else isgpu=false;


	smh = (zes_device_handle_t)dev;

	npwrdoms = 0;
	res = zesDeviceEnumPowerDomains(smh, &npwrdoms, nullptr);
	if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zesDeviceEnumPowerDomains", res);
	if (npwrdoms > 0) {
	    pwrhs.resize(npwrdoms);
	    prev_energy_uj.resize(npwrdoms);
	    prev_ts_us.resize(npwrdoms);

	    res = zesDeviceEnumPowerDomains(smh, &npwrdoms, pwrhs.data());
	    if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zesDeviceEnumPowerDomains", res);

	    // to load value into prev_energy_uj and prev_ts_us to
	    // calculate the average power consumption.
	    zes_power_energy_counter_t ecounter;
	    for (int i = 0; i < npwrdoms; ++i)
		sampleenergy(i, ecounter);
	    usleep(10*1000); // sleep 10 msec

	    zes_pwr_handle_t pwrh = getpwrh(0); // note check only the first domain

	    // old API
	    // zes_power_sustained_limit_t pSustained;
	    // zes_power_burst_limit_t pBurst;
	    // zes_power_peak_limit_t pPeak;
	    // res = zesPowerGetLimits(pwrh, &pSustained, &pBurst, &pPeak);

	    zes_power_properties_t pprop = {};
	    res = zesPowerGetProperties(pwrh, &pprop);
	    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetProperties", res);

	    enabled_powerlimit = false;

	    if(pprop.canControl > 0) {
		const int maxpCount = 10;
		zes_power_limit_ext_desc_t pSustained[maxpCount];
		uint32_t pCount;
		pCount = 0;

		res = zesPowerGetLimitsExt(pwrh, &pCount, pSustained);
		if (res != ZE_RESULT_SUCCESS) {
		    std::cout << "Warning: PowerLimit is unavailable. Disabled the fueature." << std::endl;
		} else {
		    if (pCount > maxpCount) {
			pCount = maxpCount;
			std::cout << "Warning: pCount is reset to " << pCount << std::endl;
		    }

		    for (int j=0; j<pCount; j++) {
			zes_power_limit_ext_desc_t *p;
			p = pSustained + j;

			if (p->level == ZES_POWER_LEVEL_SUSTAINED) {
			    enabled_powerlimit = true;
			}
		    }
		}
	    }
	}

	nfreqdoms = 0;
	res = zesDeviceEnumFrequencyDomains(smh, &nfreqdoms ,NULL);
	if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zesDeviceEnumFrequencyDomains", res);
	if (nfreqdoms > 0) {
	    freqhs.resize(nfreqdoms);
	    res = zesDeviceEnumFrequencyDomains(smh, &nfreqdoms, freqhs.data());
	    if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zesDeviceEnumFrequencyDomains", res);
	}


	ntempsensors = 0;
	res = zesDeviceEnumTemperatureSensors(smh, &ntempsensors ,NULL);
	if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zesDeviceEnumTemperatureSensors", res);
	if (ntempsensors > 0) {
	    temphs.resize(ntempsensors);
	    res = zesDeviceEnumTemperatureSensors(smh, &ntempsensors, temphs.data());
	    if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zesDeviceEnumTemperatureSensors", res);
	}

	if (verbose >=1 ) {
	    std::cout << "Device" << devid << " isgpu=" << isgpu;
	    std::cout << " npwrdoms=" << npwrdoms;
	    std::cout << " nfreqdoms=" << nfreqdoms;
	    std::cout << " ntempsensors=" << ntempsensors << std::endl;
	}

	if (verbose >= 2) std::cout << "IDGPowerPerDivice is constructed" << std::endl;

    }

    ~IDGPowerPerDevice() {
	if (verbose >= 2) std::cout << "IDGPowerPerDevice is destructed" << std::endl;
    }

    bool isgputype() {return isgpu;}
    uint32_t getnpwrdoms() { return npwrdoms; }
    uint32_t getnfreqdoms()  { return nfreqdoms; }
    uint32_t getntempsensors()  { return ntempsensors; }

    ze_device_handle_t getdev() {return dev; }
    zes_device_handle_t getsysmanh() {return smh; }
    bool is_powerlimit_available() { return enabled_powerlimit; }
    zes_pwr_handle_t getpwrh(int id) {
	if (id >= getnpwrdoms() ) {
		std::cout << "Warning: getpwrh(): specified id is out of the range: set it to 0" << std::endl;
		id = 0;
	}
	return pwrhs[id];
    }
    zes_freq_handle_t getfreqh(int id) {
	if (id >= getnfreqdoms() ) {
		std::cout << "Warning: getfreqh(): specified id is out of the range: set it to 0" << std::endl;
		id = 0;
	}
	return freqhs[id];
    }
    zes_temp_handle_t gettemph(int id) {
	if (id >= getntempsensors() ) {
		std::cout << "Warning: gettemph(): specified id is out of the range: set it to 0" << std::endl;
		id = 0;
	}
	return temphs[id];
    }

    // return watt
    double sampleenergy(int pwrid, zes_power_energy_counter_t& ecounter) {
	ze_result_t res;
	double watt = 0.0;

	zes_pwr_handle_t pwrh = getpwrh(pwrid);

	res = zesPowerGetEnergyCounter(pwrh, &ecounter);
	if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetEnergyCounter", res);

	double delta_us = ecounter.timestamp - prev_ts_us[pwrid];
	double delta_uj = ecounter.energy - prev_energy_uj[pwrid];
	if (delta_us > 0.0) watt = delta_uj/delta_us;

	prev_energy_uj[pwrid] = ecounter.energy;
	prev_ts_us[pwrid] = ecounter.timestamp;

	return watt;
    }
};


class IDGPowerPerDriver {
    int verbose;
    int drvid;

    ze_driver_handle_t drv;
    std::vector<IDGPowerPerDevice> devs;

public:
    IDGPowerPerDriver(ze_driver_handle_t _drv, const int _drvid, const int _ver=1) {
	ze_result_t res;
	verbose = _ver;
	drv = _drv;

	// populating devices
	std::vector<ze_device_handle_t> tmpdevs;

	uint32_t tmpdevcnt = 0;
	res = zeDeviceGet(drv, &tmpdevcnt, nullptr);
	if (res != ZE_RESULT_SUCCESS || tmpdevcnt == 0) {
	    std::cout << "ERROR: No device found!" << std::endl;
	    _ZE_ERROR_MSG("zeDeviceGet", res);
	}
	tmpdevs.resize(tmpdevcnt);

	res = zeDeviceGet(drv, &tmpdevcnt, tmpdevs.data());
	if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zeDeviceGet", res);

	for(uint32_t i = 0; i < tmpdevcnt; i++) devs.push_back(IDGPowerPerDevice(tmpdevs[i], i, verbose));


	if (verbose >= 1) std::cout << "The number of the detected devices: " << devs.size() << std::endl;


	if (verbose >= 2) std::cout << "IDGPowerPerDriver is constructed" << std::endl;
    }

    ~IDGPowerPerDriver() {
	if (verbose >= 2) std::cout << "IDGPowerPerDriver is destructed" << std::endl;
    }

    void getVersion(uint32_t &version) {
	ze_driver_properties_t prop;
	ze_result_t res = zeDriverGetProperties(drv, &prop);
	if (res != ZE_RESULT_SUCCESS ) {
	    _ZE_ERROR_MSG_NOTERMINATE("zeDriverGetProperties", res);
	}
    }

    int getndevs() { return devs.size(); }

    IDGPowerPerDevice getIDGPowerPerDevice(int devid) {
		if (devid >= getndevs()) {
			std::cout << "Warning: devid is out of the range. Set devid 0" << std::endl;
			return devs[0];
		}
	    return devs[devid];
    }
};

class IDGPower {
    bool enabled;
    int  drvselected;

    std::vector<IDGPowerPerDriver> drvs;

    int verbose;

public:
    IDGPower(const int _verbose = 1) {
	ze_result_t res;

	verbose = _verbose;
	enabled = false;

	res = zeInit(ZE_INIT_FLAG_GPU_ONLY);
	if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zeInit", res);

	std::vector<ze_driver_handle_t> tmpdrvs;

	uint32_t tmpdrvcnt = 0;
	// populate drivers
	res = zeDriverGet(&tmpdrvcnt, nullptr);
	if (res != ZE_RESULT_SUCCESS || tmpdrvcnt == 0) {
	    std::cout << "ERROR: No driver found!" << std::endl;
	    _ZE_ERROR_MSG("zeDriverGet", res);
	}
	tmpdrvs.resize(tmpdrvcnt);
	res = zeDriverGet(&tmpdrvcnt, tmpdrvs.data());
	if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG("zeDriverGet", res);

	for(uint32_t i = 0; i < tmpdrvcnt; i++)  drvs.push_back(IDGPowerPerDriver(tmpdrvs[i], i, verbose));

	if (verbose >= 1) std::cout << "The number of drivers detected: " << tmpdrvcnt << std::endl;

	enabled = true;
	drvselected = 0; // for now, it is set to zero. TODO:
			 // implement some kind of selector later
    }
    ~IDGPower() {
	if (verbose >= 2) std::cout << "IDGPower is destructed" << std::endl;
    }

    int isEnabled() { return enabled; }
    int getndevs() { return drvs[drvselected].getndevs(); }
    IDGPowerPerDevice getIDGPowerPerDevice(int devid) {
	return drvs[drvselected].getIDGPowerPerDevice(devid);
    }
};

// singleton object of IDGPower
static IDGPower *apmidg = NULL;

// protect control features
static std::mutex apmidg_mutex;

// the availablity of features


#define EXTERNC extern "C"

EXTERNC int apmidg_getndevs() {
    if (!apmidg) return -1;
    return apmidg->getndevs();
}

EXTERNC int apmidg_getnpwrdoms(int devid) {
    if (!apmidg) return -1;
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    return perdev.getnpwrdoms();
}

EXTERNC void apmidg_getpwrprops(int devid, int pwrid, int *onsubdev, int *subdevid, int *canctrl, int *deflim_mw, int *minlim_mw, int *maxlim_mw) {

    if (onsubdev) *onsubdev = -1;
    if (subdevid) *subdevid = -1;
    if (canctrl)  *canctrl = -1;
    if (deflim_mw) *deflim_mw = -1;
    if (minlim_mw) *minlim_mw = -1;
    if (maxlim_mw) *maxlim_mw = -1;

    if (!apmidg) return;

    ze_result_t res;
    zes_power_properties_t pprop = {};
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_pwr_handle_t pwrh = perdev.getpwrh(pwrid);

    res = zesPowerGetProperties(pwrh, &pprop);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetProperties", res);

    if (onsubdev) *onsubdev = (int)pprop.onSubdevice;
    if (subdevid) *subdevid = (int)pprop.subdeviceId;
    if (canctrl)  *canctrl = (int)pprop.canControl;
    if (deflim_mw) *deflim_mw = (int)pprop.defaultLimit;
    if (minlim_mw) *minlim_mw = (int)pprop.minLimit;
    if (maxlim_mw) *maxlim_mw = (int)pprop.maxLimit;

    //
    // Workaround. L0 sets defaultLimit -1 (reading a wrong sysfs file)
    // Remove this later once L0 is fixed!
    static bool msgdisplayed = false;
    std::string buf;
    std::fstream fs;
    int workaround_maxlimit=-1;
    fs.open("/sys/class/drm/card0/device/hwmon/hwmon1/power1_max_default", std::ios::in);
    if (fs) {
	fs >> buf;
	fs.close();
	workaround_maxlimit = stoi(buf) / 1000; // uw to mw
    }
    if (canctrl>0 && (int)pprop.defaultLimit == -1 && workaround_maxlimit > 0) {
	if (deflim_mw) *deflim_mw = workaround_maxlimit;
	if (!msgdisplayed) {
	    std::cout << "apmidg_getpwrprops: deflim_mw workaround applied" << std::endl;
	    msgdisplayed = true;
	}
    }
}

static void prlevel(int level) {
    switch(level) {
	case ZES_POWER_LEVEL_SUSTAINED:
	    std::cout << "Sustained" << std::endl;
	    break;
	case ZES_POWER_LEVEL_PEAK:
	    std::cout << "Peak" << std::endl;
	    break;
	case ZES_POWER_LEVEL_BURST:
	    std::cout << "Burst" << std::endl;
	    break;
	case ZES_POWER_LEVEL_INSTANTANEOUS:
	    std::cout << "Instantaneous" << std::endl;
	    break;
	default:
	    std::cout << "Invalid Power Limit" << std::endl;
    }
}


EXTERNC void apmidg_getpwrlim(int devid, int pwrid, int *lim_mw) {// sustained only
    if (lim_mw) *lim_mw = -1;
    if (!apmidg) return;

    ze_result_t res;
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    if (!perdev.is_powerlimit_available()) return;
    zes_pwr_handle_t pwrh = perdev.getpwrh(pwrid);

#if 0 // OLD API
    zes_power_sustained_limit_t pSustained;
    zes_power_burst_limit_t pBurst;
    zes_power_peak_limit_t pPeak;
    res = zesPowerGetLimits(pwrh, &pSustained, &pBurst, &pPeak);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetLimits", res);
    if (lim_mw) *lim_mw = pSustained.power;
#endif

    const int maxpCount = 10;
    zes_power_limit_ext_desc_t pSustained[maxpCount];
    uint32_t pCount;
    pCount = 0;
    res = zesPowerGetLimitsExt(pwrh, &pCount, pSustained);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetLimitsExt", res);
    res = zesPowerGetLimitsExt(pwrh, &pCount, pSustained);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetLimitsExt", res);

    if (pCount > maxpCount) {
	pCount = maxpCount;
	std::cout << "Warning: pCount is reset to " << pCount << std::endl;
    }

    int lim_mw_queried = -1;

    for (int j=0; j<pCount; j++) {
	zes_power_limit_ext_desc_t *p;
	p = pSustained + j;

	if (p->level == ZES_POWER_LEVEL_SUSTAINED) {
	    lim_mw_queried = p->limit;
	}
    }

    if (lim_mw && lim_mw_queried > 0) {
	*lim_mw = lim_mw_queried;
    } else {
	std::cout << "Warning: apmidg_getpwrlim found no target power level." << std::endl;
    }
}

EXTERNC void apmidg_setpwrlim(int devid, int pwrid, int lim_mw) { // sustained only
    if (!apmidg) return;

    ze_result_t res;
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    if (!perdev.is_powerlimit_available()) return;
    zes_pwr_handle_t pwrh = perdev.getpwrh(pwrid);

#if 0 // OLD API
    zes_power_sustained_limit_t pSustained;

    apmidg_mutex.lock();
    res = zesPowerGetLimits(pwrh, &pSustained, NULL, NULL);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetLimits", res);

    pSustained.power = lim_mw;

    res = zesPowerSetLimits(pwrh, &pSustained, NULL, NULL);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerSetLimits", res);
    apmidg_mutex.unlock();
#endif

    const int maxpCount = 10;
    zes_power_limit_ext_desc_t pSustained[maxpCount];
    uint32_t pCount;
    pCount = 0;
    res = zesPowerGetLimitsExt(pwrh, &pCount, pSustained);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetLimitsExt", res);

    res = zesPowerGetLimitsExt(pwrh, &pCount, pSustained);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerGetLimitsExt", res);

    if (pCount > maxpCount) {
	pCount = maxpCount;
	std::cout << "Warning: pCount is reset to " << pCount << std::endl;
    }

    bool found_sustained = false;

    for (int j=0; j<pCount; j++) {
	zes_power_limit_ext_desc_t *p;
	p = pSustained + j;
	if (p->level == ZES_POWER_LEVEL_SUSTAINED) {
	    found_sustained = true;
	    p->limit = lim_mw;
	}
    }

    if (found_sustained) {
	res = zesPowerSetLimitsExt(pwrh, &pCount, pSustained);
	if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesPowerSetLimitsExt", res);
    } else {
	std::cout << "Warning: apmidg_setpwrlim found no target power level" << std::endl;
    }
}

EXTERNC void apmidg_readenergy(int devid, int pwrid, uint64_t *energy_uj, uint64_t *ts_us) {
    if (energy_uj)  *energy_uj = -1;
    if (ts_us) *ts_us = -1;
    if (!apmidg) return;

    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_pwr_handle_t pwrh = perdev.getpwrh(pwrid);

    // sampleenergy
    zes_power_energy_counter_t ecounter;
    perdev.sampleenergy(pwrid, ecounter);

    if (energy_uj) *energy_uj = ecounter.energy;
    if (ts_us) *ts_us = ecounter.timestamp;
}

EXTERNC double apmidg_readpoweravg(int devid, int pwrid) {
    double watt = 0.0;
    if (!apmidg) return watt;

    ze_result_t res;
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_pwr_handle_t pwrh = perdev.getpwrh(pwrid);
    zes_power_energy_counter_t ecounter;

    apmidg_mutex.lock();
    watt = perdev.sampleenergy(pwrid, ecounter);
    apmidg_mutex.unlock();

    return watt;
}


EXTERNC int apmidg_getnfreqdoms(int devid) {
    if (!apmidg) return -1;

    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    return perdev.getnfreqdoms();
}


EXTERNC void apmidg_getfreqprops(int devid, int freqid, int *onsubdev,
				int *subdevid, int *canctrl,
				 double *min_MHz, double *max_MHz)
{
    if (onsubdev) *onsubdev = -1;
    if (subdevid) *subdevid = -1;
    if (canctrl)  *canctrl = -1;
    if (min_MHz) *min_MHz = -1.0;
    if (max_MHz) *max_MHz = -1.0;
    if (!apmidg) return;

    ze_result_t res;
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_freq_handle_t freqh = perdev.getfreqh(freqid);
    zes_freq_properties_t fprop;

    res = zesFrequencyGetProperties(freqh, &fprop);
    if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG_NOTERMINATE("zesFrequencyGetProperties", res);
    if (onsubdev) *onsubdev = fprop.onSubdevice;
    if (subdevid) *subdevid = fprop.subdeviceId;
    if (canctrl)  *canctrl = fprop.canControl;
    if (min_MHz) *min_MHz = fprop.min;
    if (max_MHz) *max_MHz = fprop.max;

}

EXTERNC void apmidg_getfreqlims(int devid, int freqid,
				 double *min_MHz, double *max_MHz) {
    if (min_MHz) *min_MHz = -1.0;
    if (max_MHz) *max_MHz = -1.0;

    if (!apmidg) return;

    ze_result_t res;
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_freq_handle_t freqh = perdev.getfreqh(freqid);
    zes_freq_range_t frange;

    res = zesFrequencyGetRange(freqh, &frange);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesFrequencyGetRange", res);

    if (min_MHz) *min_MHz = frange.min;
    if (max_MHz) *max_MHz = frange.max;
}

EXTERNC void apmidg_setfreqlims(int devid, int freqid,
				 double min_MHz, double max_MHz) {
    if (!apmidg) return;

    ze_result_t res;
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_freq_handle_t freqh = perdev.getfreqh(freqid);
    zes_freq_range_t frange;

    frange.min = min_MHz;
    frange.max = max_MHz;

    apmidg_mutex.lock();
    res = zesFrequencySetRange(freqh, &frange);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesFrequencySetRange", res);
    apmidg_mutex.unlock();

}

EXTERNC void apmidg_readfreq(int devid, int freqid, double *actual_MHz) {
    if (actual_MHz) *actual_MHz = -1.0;
    if (!apmidg) return;

    ze_result_t res;
    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_freq_handle_t freqh = perdev.getfreqh(freqid);
    zes_freq_state_t fstate;

    res = zesFrequencyGetState(freqh, &fstate);
    if (res != ZE_RESULT_SUCCESS)  _ZE_ERROR_MSG_NOTERMINATE("zesFrequencyGetState", res);
    if (actual_MHz) *actual_MHz = fstate.actual;

}


EXTERNC int apmidg_getntempsensors(int devid) {
    if (!apmidg) return -1;

    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    return perdev.getntempsensors();
}

EXTERNC void apmidg_gettempprops(int devid, int tempid, int *onsubdev,
				 int *subdevid,  int *type) {
    if (onsubdev) *onsubdev = -1;
    if (subdevid) *subdevid = -1;
    if (type) *type = -1;

    if (!apmidg) return;

    ze_result_t res;

    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_temp_handle_t temph = perdev.gettemph(tempid);
    zes_temp_properties_t tprop;

    res = zesTemperatureGetProperties(temph, &tprop);
    if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG_NOTERMINATE("zesTemperatureGetProperties", res);
    if (onsubdev) *onsubdev = tprop.onSubdevice;
    if (subdevid) *subdevid = tprop.subdeviceId;
    if (type) *type = tprop.type;
}

EXTERNC const char* apmidg_sensortype_str(int type)
{
    const char *pre = "ZES_TEMP_SENSORS_";
    const char *typestr = str_zes_temp_sensors_t(type);

    if (strncmp(typestr, pre, strlen(pre)) == 0 && strlen(typestr) > strlen(pre))
	return typestr+strlen(pre);

    return "UNKNOWN";
}


EXTERNC void apmidg_readtemp(int devid, int tempid, double *temp_C) {
    if (temp_C) *temp_C = -1.0;
    if (!apmidg) return;

    ze_result_t res;

    IDGPowerPerDevice perdev = apmidg->getIDGPowerPerDevice(devid);
    zes_temp_handle_t temph = perdev.gettemph(tempid);

    if (temp_C) {
	res = zesTemperatureGetState(temph, temp_C);
	if (res != ZE_RESULT_SUCCESS) _ZE_ERROR_MSG_NOTERMINATE("zesTemperatureGetState", res);
    }
}


EXTERNC int apmidg_init(int verbose)
{
    int ret;

    if(setenv("ZES_ENABLE_SYSMAN", "1", 1) != 0) {
	perror("setenv() failed");
	exit(1);
    }

#if 0
    const char *e = getenv("ZES_ENABLE_SYSMAN");
    if (!(e && e[0] == '1'))  {
	std::cout << "Error: please set ZES_ENABLE_SYSMAN to 1" << std::endl;
	exit(1);
    }
#endif

    if (apmidg) {
	std::cout << "Warning: apmidg is already initialized";
	return -1;
    }

    apmidg = new IDGPower(verbose); // the arg is the verbose level
    if (! (apmidg && apmidg->isEnabled()) ) {
	return -1;
    }

    return 0;
}

EXTERNC void apmidg_finish()
{
    if (apmidg)   delete apmidg;
    apmidg = NULL;
}
