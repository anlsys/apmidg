#!/usr/bin/env python

"""
COOLR Intel Discrete GPU power management module

Requirements:
- Intel Discrete GPUs
- oneAPI runtime environment (level_zero.so)
- libapmidg.so
"""

from ctypes import *
import time

class rtype_getpwrprops:
    def __init__(self, onsubdev, subdevid, canctrl, deflim_mw, minlim_mw, maxlim_mw):
        self.onsubdev = onsubdev.value
        self.subdevid = subdevid.value
        self.canctrl = canctrl.value
        self.deflim_mw = deflim_mw.value
        self.minlim_mw = minlim_mw.value
        self.maxlim_mw = maxlim_mw.value

class rtype_readenergy:
    def __init__(self, energy_uj, ts_usec):
        self.energy_uj = energy_uj.value
        self.ts_usec = ts_usec.value

class clr_apmidg:
    """
    """

    def __init__(self):
        self.apm = CDLL("libapmidg.so")
        ret = self.apm.apmidg_init()


        # define argtypes here if needed
        self.func_getpwrprops = self.apm.apmidg_getpwrprops
        self.func_getpwrprops.argtypes = [c_int, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_int)]
        #
        self.func_getpwrlim = self.apm.apmidg_getpwrlim
        self.func_getpwrlim.argstypes = [c_int, c_int, POINTER(c_int)]
        #
        self.func_readenergy = self.apm.apmidg_readenergy
        self.func_readenergy.argstypes = [c_int, c_int, POINTER(c_ulonglong), POINTER(c_ulonglong)]
        #


    def __del__(self):
        self.apm.apmidg_finish()

    def getndevs(self):
        return self.apm.apmidg_getndevs()


    #
    # Power domain
    #

    def getnpwrdoms(self, devid):
        return self.apm.apmidg_getnpwrdoms(devid)

    def getpwrprops(self, devid, pwrid):
        onsubdev = c_int()
        subdevid = c_int()
        canctrl = c_int()
        deflim_mw = c_int()
        minlim_mw = c_int()
        maxlim_mw = c_int()

        self.func_getpwrprops(devid, pwrid, byref(onsubdev), byref(subdevid), byref(canctrl), byref(deflim_mw), byref(minlim_mw), byref(maxlim_mw))
        return rtype_getpwrprops(onsubdev, subdevid, canctrl, deflim_mw, minlim_mw, maxlim_mw)

    def getpwrlim(self, devid, pwrid):
        lim_mw = c_int()
        self.func_getpwrlim(devid, pwrid, byref(lim_mw))
        return lim_mw.value

    def setpwrlim(self, devid, pwrid, lim_mw):
        self.apm.apmidg_setpwrlim(devid, pwrid, lim_mw)

    def readenergy(self, devid, pwrid):
        energy_uj = c_ulonglong()
        ts_usec = c_ulonglong()
        self.func_readenergy(devid, pwrid, byref(energy_uj), byref(ts_usec))
        return rtype_readenergy(energy_uj, ts_usec)

    #
    # Frequency domain
    #

    def getnfreqdoms(self, devid):
        return self.apm.apmidg_getnfreqdoms(devid)

    #
    # Temperature sensor
    #

    def getntempsensors(self, devid):
        return self.apm.apmidg_getntempsensors(devid)

if __name__ == '__main__':
    pm = clr_apmidg()
    ndevs = pm.getndevs()

    for devid in range(0, ndevs):
        npwrdoms = pm.getnpwrdoms(devid)
        nfreqdoms = pm.getnfreqdoms(devid)
        ntempsensors = pm.getntempsensors(devid)
        print("devid=%d: npwrdoms=%d nfreqdoms=%d ntempsensors=%d" % (devid, npwrdoms, nfreqdoms, ntempsensors))

        fspstr = " "*9 # just for formatting
        for pwrid in range(0, npwrdoms):
            pwrprops = pm.getpwrprops(devid, pwrid)
            curpwrlim_mw = pm.getpwrlim(devid, pwrid)
            print("%sdeflim_mw=%d curpwrlim_mw=%d" % (fspstr, pwrprops.deflim_mw, curpwrlim_mw))
            target_mw = curpwrlim_mw / 4 * 3
            pm.setpwrlim(devid, pwrid, target_mw)
            tmppwrlim_mw = pm.getpwrlim(devid, pwrid)

            if target_mw == tmppwrlim_mw:
                print("%stesting powercap: passed" % fspstr)
            else:
                print("%stesting powercap: failed" % fspstr)
            pm.setpwrlim(devid, pwrid, curpwrlim_mw)

    print("")
    print("Test monitoring")

    for t in range(0,5):
        for devid in range(0, ndevs):
            npwrdoms = pm.getnpwrdoms(devid)
            nfreqdoms = pm.getnfreqdoms(devid)
            ntempsensors = pm.getntempsensors(devid)
            for pwrid in range(0, npwrdoms):
                et = pm.readenergy(devid, pwrid)
                print("devid%d/pwrid%d: %d usec   %d uJ" % (devid, pwrid, et.ts_usec, et.energy_uj))
        time.sleep(1)
