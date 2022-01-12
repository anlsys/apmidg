APMIDG: Argo Power Management Glue Layer for Intel Discrete GPUs

Requirements:
- Intel Discrete GPUs
- oneAPI environment (header files and shared libraries)
- cmake 3.9 or later

To build shared libraries and testcodes
---------------------------------------

	$ mkdir build && cd build
	$ cmake -DCMAKE_INSTALL_PREFIX:PATH=__LIBAPMIDG_INSTALL_PATH__ ..
	$ make
	$ pip install .

	NOTE: please replace __LIBAPMIDG_INSTALL_PATH__ with your preferred path.

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
