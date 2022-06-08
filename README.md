APMIDG: Argo Power Management Glue Layer for Intel Discrete GPUs

Requirements:
- Intel Discrete GPUs
- oneAPI environment (header files and shared libraries)
- cmake 3.9 or later

To build shared libraries and testcodes
---------------------------------------

	$ mkdir build && cd build
	$ cmake -DCMAKE_INSTALL_PREFIX:PATH=__LIBAPMIDG_INSTALL_PATH__ ..
	$ make install

	NOTE: please replace __LIBAPMIDG_INSTALL_PATH__ with your preferred path.

To run test codes
-----------------

	$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:__LIBAPMIDG_INSTALL_PATH__/lib64
	$ export PATH=$PATH:__LIBAPMIDG_INSTALL_PATH__/bin

	C-based testcode
	$ apmidgstats

	Python binding demo
	$ python3
	>>> import pyapmidg
	>>> pm = pyapmidg.clr_apmidg()
	>>> pm.readpoweravg()  # read the average power of domain0 on device0
	xx.x
	>>> pm.getndevs() # return the number of GPUs
	x
	>>> pm.getnpwrdoms(0) # return the number of the power domains on device0. Note the number starts at 0
	x
	>>> pm.getnfreqdoms(1) # return the number of the freq domains on device1
	x
	>>> pm.getntempsensors(0) # return the number of the temp sensors on device0
	x
	>>> pm.readpoweravg(1,0)  # read the average power of domain0 on device1
	>>> flims = pm.getfreqlims(1,0) # read the current freq limit of domain0 on device1
	>>> flims.max_MHz
	xxxx.x
	>>> flims.max_MHz = xxxx
	>>>> pm.setfreqlims(1, 0, flims.min_MHz, flims.max_MHz)
	>>> pm.reset2default() # reset back to the default setting



NOTE:
- See src/pyapmidg/demo_monitor_articus for Python API usages
- C examples are available in src/c_examples

--
Contact: Kazutomo Yoshii <kazutomo@mcs.anl.gov>
