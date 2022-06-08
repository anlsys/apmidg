#include "libapmidg.h"
#include <stdio.h>
#include <unistd.h>

static void reportpoweravgall()
{
    int ndevs = apmidg_getndevs();

    // iterate GPUs
    for (int di=0; di<ndevs; di++) {
	int npwrdoms = apmidg_getnpwrdoms(di);
	printf("dev%d: ", di);
	for (int pi=0; pi<npwrdoms; pi++) {
	    double watt = apmidg_readpoweravg(di, pi);
	    printf("domain%d=%5.1lf   ", pi, watt);
	}
	printf("\n");
    }
}

int main()
{
    int n = 5;
    int verbose = 1;
    if(apmidg_init(verbose) != 0) return 1;

    printf("Read the poweravg of all power domains for all GPUs. Unit is Watt\n\n");
    for (int i = 0; i < n; i++) {
	reportpoweravgall();
	sleep(1);
    }
    
    return 0;
}
