APMIDG: Argo Power Management Glur Layer for Intel Discrete GPUs


Requirements:
- Intel Discrete GPUs
- oneAPI runtime environment (level_zero.so)


To build shared libraries and testcodes
---------------------------------------

	First, you need to set up Intel oneAPI environment
	$ module load oneapi
	or 
	$ source $ONEAPIAPTH/setvars.sh

	Then, type
	$ make

To run test codes
-----------------

	$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
	$ export ZES_ENABLE_SYSMAN=1

	C-based testcode
	$ ./testlibapmidg

	Python binding demo
	python clr_apmidg.py


--
Technical contact: Kazutomo Yoshii <kazutomo@mcs.anl.gov>
