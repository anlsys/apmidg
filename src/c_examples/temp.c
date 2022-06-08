#include "libapmidg.h"
#include <stdio.h>
#include <unistd.h>

// note: a convenient func for this will be added to the library
static const char *sensorname(int devid, int sid)
{
    int onsubdev, subdevid, stype;
    apmidg_gettempprops(devid, sid, &onsubdev,
			&subdevid,  &stype);
    return apmidg_sensortype_str(stype);
}

static void reporttempall()
{
    int ndevs = apmidg_getndevs();
	
    // iterate GPUs
    for (int di=0; di<ndevs; di++) {
	int ntempsensors = apmidg_getntempsensors(di);
	printf("dev%d: ", di);
	for (int ti=0; ti<ntempsensors; ti++) {
	    double temp_C;

	    apmidg_readtemp(di, ti, &temp_C);
	    printf("domain%d(%s)=%5.1lf   ", ti, sensorname(di, ti), temp_C);
	}
	printf("\n");
    }
}

int main()
{
    int n = 5;
    int verbose = 1;
    if(apmidg_init(verbose) != 0) return 1;

    printf("Read the temperature of all domains for available GPUs. Unit is C\n\n");
    for (int i = 0; i < n; i++) {
	reporttempall();
	sleep(1);
    }

    apmidg_finish();
    
    return 0;
}
