#include "libapmidg.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int verbose = 1;
    int ndevs;
    
    if(apmidg_init(verbose) != 0) return 1;

    for (int devid = 0; devid < apmidg_getndevs(); devid++) {
	printf("devid=%d\n", devid);
	
	for (int pwrid = 0; pwrid < apmidg_getnpwrdoms(devid); pwrid++) {
	    int canctrl = 0;
	    int deflim_mw = 0;
	    
	    apmidg_getpwrprops(devid, pwrid, NULL, NULL, &canctrl, &deflim_mw, NULL, NULL);
	    if (canctrl) {
		int curlim_mw;
		int last_lim_mw;
		apmidg_getpwrlim(devid, pwrid, &curlim_mw);
		printf("  [%d] deflim_mw=%d curlim_mw=%d\n", pwrid, deflim_mw, curlim_mw);
		last_lim_mw = deflim_mw;
		for (int target_lim_mw=deflim_mw; ; target_lim_mw-=(100*1000)) {
		    apmidg_setpwrlim(devid, pwrid, target_lim_mw);
		    apmidg_getpwrlim(devid, pwrid, &curlim_mw);
		    if (target_lim_mw != curlim_mw) {
			printf("    lowest limit_mw is %d\n", last_lim_mw);
			apmidg_setpwrlim(devid, pwrid, deflim_mw); // revert back to the default
			break;
		    }
		    last_lim_mw = target_lim_mw;
		}
		
	    }
	}
    }
    apmidg_finish();
    
    return 0;
}
