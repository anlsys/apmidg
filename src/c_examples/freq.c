#include "libapmidg.h"
#include <stdio.h>
#include <unistd.h>

static void reportfreqall()
{
    int ndevs = apmidg_getndevs();

    // iterate GPUs
    for (int di=0; di<ndevs; di++) {
	int nfreqdoms = apmidg_getnfreqdoms(di);
	printf("dev%d: ", di);
	// iterate the freq domains
	for (int fi=0; fi<nfreqdoms; fi++) {
	    double actual_MHz;
	    apmidg_readfreq(di, fi, &actual_MHz);
	    printf("domain%d=%5.1lf   ", fi, actual_MHz);
	}
	printf("\n");
    }
}

int main()
{
    int n = 5;
    int verbose = 1;
    if(apmidg_init(verbose) != 0) return 1;

    printf("Read the frequency of all domains for all GPUs. Unit is MHz\n\n");
    for (int i = 0; i < n; i++) {
	reportfreqall();
	sleep(1);
    }

    apmidg_finish();

    return 0;
}
