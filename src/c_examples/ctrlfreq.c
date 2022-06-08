#include "libapmidg.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void listfreqrange()
{
    int ndevs = apmidg_getndevs();

    printf("[Default setting]\n");
    for (int di=0; di<ndevs; di++) {
	int nfreqdoms = apmidg_getnfreqdoms(di);
	for (int fi=0; fi<nfreqdoms; fi++) {
	    int onsubdev, subdevid, canctrl;
	    double min_MHz, max_MHz;
	    apmidg_getfreqprops(di, fi, &onsubdev,
				&subdevid, &canctrl,
				&min_MHz, &max_MHz);
	    printf("dev=%d freqdom=%d onsubdev=%d subdevid=%d canctrl=%d min_MHz=%.1f max_MHz=%.1f\n",
		    di, fi, onsubdev, subdevid, canctrl, min_MHz, max_MHz);
	}
    }
}

static void reset2freqdefault()
{
    int ndevs = apmidg_getndevs();
    printf("Reset back to default\n");
    for (int di=0; di<ndevs; di++) {
	int nfreqdoms = apmidg_getnfreqdoms(di);
	for (int fi=0; fi<nfreqdoms; fi++) {
	    int onsubdev, subdevid, canctrl;
	    double min_MHz, max_MHz;
	    apmidg_getfreqprops(di, fi, &onsubdev,
			    &subdevid, &canctrl,
			    &min_MHz, &max_MHz);
	    apmidg_setfreqlims(di, fi, min_MHz, max_MHz);
	}
    }
}

static void listcurfreq()
{
    int ndevs = apmidg_getndevs();

    printf("[Current setting]\n");
    for (int di=0; di<ndevs; di++) {
	int nfreqdoms = apmidg_getnfreqdoms(di);
	for (int fi=0; fi<nfreqdoms; fi++) {
	    double tmp_min_MHz, tmp_max_MHz;
	    apmidg_getfreqlims(di, fi, &tmp_min_MHz, &tmp_max_MHz);
	    printf("dev=%d freqdom=%d limit_min_MHz=%.1f limit_max_MHz=%.1f\n",
		    di, fi, tmp_min_MHz, tmp_max_MHz);
	}
    }
}

static void setfreq(int maxMHz)
{
    int ndevs = apmidg_getndevs();

    for (int di=0; di<ndevs; di++) {
	int nfreqdoms = apmidg_getnfreqdoms(di);
	for (int fi=0; fi<nfreqdoms; fi++) {
	    double tmp_min_MHz, tmp_max_MHz;
	    apmidg_getfreqlims(di, fi, &tmp_min_MHz, &tmp_max_MHz);
	    apmidg_setfreqlims(di, fi, tmp_min_MHz, maxMHz);
	}
    }
}






int main(int argc, char *argv[])
{
    int verbose = 1;
    int maxMHz = 0;
    
    if(apmidg_init(verbose) != 0) return 1;

    if (argc >= 2) {
	maxMHz = atoi(argv[1]);
    }

    if (maxMHz > 0) {
	setfreq(maxMHz);
	listcurfreq();
	reset2freqdefault();
    } else {
	listfreqrange();
    }

    apmidg_finish();
    
    return 0;
}
