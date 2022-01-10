#include "libapmidg.h"

#include <stdio.h>


int main()
{
    if(apmidg_init() != 0) {
	printf("Failed to initialize\n");
	return 1;
    }

    int ndevs = apmidg_getndevs();
    printf("ndevs=%d\n", ndevs);

    // detection and check functionality
    for (int di=0; di<ndevs; di++) {
	int npwrdoms = apmidg_getnpwrdoms(di);
	int nfreqdoms = apmidg_getnfreqdoms(di);
	int ntempsensors = apmidg_getntempsensors(di);

	printf("dev%d: npwrdoms=%d nfreqdoms=%d\n", di,
	       npwrdoms, nfreqdoms);
	for (int pi=0; pi<npwrdoms; pi++) {
	    int onsubdev, subdevid, canctrl, deflim_mw; // minlim_mw, maxlim_mw
	    int curlim_mw; // current limit

	    apmidg_getpwrprops(di, pi, &onsubdev, &subdevid,
			       &canctrl, &deflim_mw, NULL, NULL);

	    apmidg_getpwrlim(di, pi, &curlim_mw);

	    printf("      pwrdom=%d onsubdev=%d subdevid=%d canctrl=%d deflim_mw=%d curlim_mw=%d\n",
		   pi, onsubdev, subdevid, canctrl, deflim_mw, curlim_mw);
	    int targetlim_mw =  curlim_mw/2;
	    apmidg_setpwrlim(di, pi, targetlim_mw);
	    int prevlim_mw = curlim_mw;
	    apmidg_getpwrlim(di, pi, &curlim_mw);
	    if (targetlim_mw == curlim_mw)
		printf("               testing powercap: passed\n");
	    else
		printf("               testing powercap: failed (possibly insufficient persmission for hwmon\n");
	    apmidg_setpwrlim(di, pi, prevlim_mw); // revert back

	    // test reader
	    uint64_t energy, timestamp;
	    apmidg_readenergy(di, pi, &energy, &timestamp);
	    printf("               energy=%lu uJ timestamp=%lu usec\n",
		   energy, timestamp);

	}
	for (int fi=0; fi<nfreqdoms; fi++) {
	    int onsubdev, subdevid, canctrl;
	    double min_MHz, max_MHz;
	    apmidg_getfreqprops(di, fi, &onsubdev,
				&subdevid, &canctrl,
				&min_MHz, &max_MHz);
	    printf("      freqdom=%d onsubdev=%d subdevid=%d canctrl=%d min_MHz=%.1f max_MHz=%.1f\n",
		   fi, onsubdev, subdevid, canctrl, min_MHz, max_MHz);


	    double target_min_MHz = min_MHz + 100;
	    double target_max_MHz = max_MHz - 200;
	    apmidg_setfreqlims(di, fi, target_min_MHz, target_max_MHz);
	    double tmp_min_MHz, tmp_max_MHz;
	    apmidg_getfreqlims(di, fi, &tmp_min_MHz, &tmp_max_MHz);
	    if ((target_min_MHz==tmp_min_MHz)&&(target_max_MHz==tmp_max_MHz))
		printf("               testing freqset: passed\n");
	    else
		printf("               testing freqset: failed (possibly insufficient persmission for hwmon\n");
	    // revert back to the default
	    apmidg_setfreqlims(di, fi, min_MHz, max_MHz);

	    double actual_MHz;
	    apmidg_readfreq(di, fi, &actual_MHz);
	    printf("               actual_MHz=%.1f MHz\n", actual_MHz);
	}
	for (int ti=0; ti<ntempsensors; ti++) {
	    int onsubdev, subdevid, type;
	    double temp_C;
	    
	    apmidg_gettempprops(di, ti, &onsubdev,
				&subdevid,  &type);

	    apmidg_readtemp(di, ti, &temp_C);
	    
	    printf("      tempid=%d onsubdev=%d subdevid=%d type=%d:%s temp_C=%.1f\n",   ti, onsubdev, subdevid, type,  apmidg_sensortype_str(type), temp_C);

	}
    }
    apmidg_finish();

    return 0;
}
