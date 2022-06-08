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
	$ testlibapmidg

	Python binding demo
	$ python3
	>>> import pyapmidg
	>>> pm = pyapmidg.clr_apmidg()
	>>> pm.readpoweravg()  # read the average power of the default domain on the default
	xx.x


NOTE:
- See src/pyapmidg/demo_monitor_articus for Python API usages
- C examples are available in src/c_examples

--
Contact: Kazutomo Yoshii <kazutomo@mcs.anl.gov>
