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
import keypress

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

class rtype_getfreqprops:
    def __init__(self, onsubdev, subdevid, canctrl, min_MHz, max_MHz):
        self.onsubdev = onsubdev.value
        self.subdevid = subdevid.value
        self.canctrl = canctrl.value
        self.min_MHz = min_MHz.value
        self.max_MHz = max_MHz.value

class rtype_getfreqlims:
    def __init__(self, min_MHz, max_MHz):
        self.min_MHz = min_MHz.value
        self.max_MHz = max_MHz.value

class rtype_gettempprops:
    def __init__(self, onsubdev, subdevid, sensortype):
        self.onsubdev = onsubdev.value
        self.subdevid = subdevid.value
        self.sensortype = sensortype.value


class clr_apmidg:
    """The clr_apmidg class provides APIs for reading energy/power
    consumption and temperatures and controlling hardware power
    capping on Intel discrete GPUs via the apmidg library that
    communicates with Intel GPUs via Intel Level Zero API.

    The function basictest() and monitor() below are an example of
    this class.
    """

    def __init__(self, verbose =1):
        self.apm = CDLL("libapmidg.so")
        ret = self.apm.apmidg_init(verbose)

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

        self.ndevs = self.getndevs()
        self.prev_e = []
        for devid in range(0, self.ndevs):
            npwrdoms = self.getnpwrdoms(devid)
            tmp = []
            for pwrid in range(0, npwrdoms):
                e = self.readenergy(devid, pwrid)
                tmp.append(e)
                # print(devid, pwrid, e.energy_uj, e.ts_usec)
            self.prev_e.append(tmp)
        time.sleep(0.1) # this is a workaround for readpoweravg() to be called immediately after this object is created

        #
        self.func_getfreqprops = self.apm.apmidg_getfreqprops
        self.func_getfreqprops.argtypes = [c_int, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_double), POINTER(c_double)]
        #
        self.func_getfreqlims = self.apm.apmidg_getfreqlims
        self.func_getfreqlims.argtypes = [c_int, c_int, POINTER(c_double), POINTER(c_double)]
        #
        self.func_setfreqlims = self.apm.apmidg_setfreqlims
        self.func_setfreqlims.argtypes = [c_int, c_int, c_double, c_double]
        #
        self.func_readfreq = self.apm.apmidg_readfreq
        self.func_readfreq.argtypes = [c_int, c_int, POINTER(c_double)]
        #
        self.func_gettempprops = self.apm.apmidg_gettempprops
        self.func_gettempprops.argtypes = [c_int, c_int, POINTER(c_int),
                                           POINTER(c_int), POINTER(c_int)]
        #
        self.func_readtemp = self.apm.apmidg_readtemp
        self.func_readtemp.argtypes = [c_int, c_int, POINTER(c_double)]
        #

    def __del__(self):
        self.apm.apmidg_finish()

    def getndevs(self):
        return self.apm.apmidg_getndevs()


    #
    # Power domain
    #

    def getnpwrdoms(self, devid=0):
        return self.apm.apmidg_getnpwrdoms(devid)

    def getpwrprops(self, devid=0, pwrid=0):
        onsubdev = c_int()
        subdevid = c_int()
        canctrl = c_int()
        deflim_mw = c_int()
        minlim_mw = c_int()
        maxlim_mw = c_int()

        self.func_getpwrprops(devid, pwrid, byref(onsubdev), byref(subdevid), byref(canctrl), byref(deflim_mw), byref(minlim_mw), byref(maxlim_mw))
        return rtype_getpwrprops(onsubdev, subdevid, canctrl, deflim_mw, minlim_mw, maxlim_mw)

    def getpwrlim(self, devid=0, pwrid=0):
        lim_mw = c_int()
        self.func_getpwrlim(devid, pwrid, byref(lim_mw))
        return lim_mw.value

    def setpwrlim(self, devid, pwrid, lim_mw):
        self.apm.apmidg_setpwrlim(devid, pwrid, lim_mw)

    def readenergy(self, devid=0, pwrid=0):
        energy_uj = c_ulonglong()
        ts_usec = c_ulonglong()
        self.func_readenergy(devid, pwrid, byref(energy_uj), byref(ts_usec))
        return rtype_readenergy(energy_uj, ts_usec)

    def diffenergy(self, prev_energy, cur_energy):
        return (cur_energy.energy_uj - prev_energy.energy_uj)/(cur_energy.ts_usec - prev_energy.ts_usec)

    def readpoweravg(self, devid=0, pwrid=0):
        cur_e = self.readenergy(devid, pwrid)
        p = self.diffenergy(self.prev_e[devid][pwrid], cur_e)
        self.prev_e[devid][pwrid] = cur_e
        return p

    #
    # Frequency domain
    #

    def getnfreqdoms(self, devid=0):
        return self.apm.apmidg_getnfreqdoms(devid)

    def getfreqprops(self, devid=0, freqid=0):
        onsubdev = c_int()
        subdevid = c_int()
        canctrl = c_int()
        min_MHz = c_double()
        max_MHz = c_double()

        self.func_getfreqprops(devid, freqid, byref(onsubdev), byref(subdevid), byref(canctrl), byref(min_MHz), byref(max_MHz))
        return rtype_getfreqprops(onsubdev, subdevid, canctrl,min_MHz, max_MHz)

    def getfreqlims(self, devid=0, freqid=0):
        min_MHz = c_double()
        max_MHz = c_double()

        self.func_getfreqlims(devid, freqid, byref(min_MHz), byref(max_MHz))
        return rtype_getfreqlims(min_MHz, max_MHz)

    def setfreqlims(self, devid, freqid, min_MHz, max_MHz):
        self.func_setfreqlims(devid, freqid, min_MHz, max_MHz)

    def readfreq(self, devid=0, freqid=0):
        actual_MHz = c_double()
        self.func_readfreq(devid, freqid, byref(actual_MHz))
        return actual_MHz.value


    #
    # Temperature sensor
    #

    def getntempsensors(self, devid=0):
        return self.apm.apmidg_getntempsensors(devid)

    def sensortype2str(self, typeid):
        labels = ["GLOBAL", "GPU", "MEMORY", "GLOBAL_MIN", "GPU_MIN", "MEMORY_MIN"]

        if (typeid >= len(lables)):
            return "UNKNOWN"

        return labels[typeid]

    def gettempprops(self, devid=0, tempid=0):
        onsubdev = c_int()
        subdevid = c_int()
        sensortype = c_int()
        self.func_gettempprops(devid, tempid, byref(onsubdev), byref(subdevid), byref(sensortype))
        return rtype_gettempprops(onsubdev, subdevid, sensortype)

    def readtemp(self, devid=0, tempid=0):
        temp_C = c_double()
        self.func_readtemp(devid, tempid, byref(temp_C))
        return temp_C.value

    #
    # reset2default
    #

    def reset2default(self, resetfreq=True, resetpwr=False):
        for devid in range(0, self.ndevs):
            npwrdoms = self.getnpwrdoms(devid)
            nfreqdoms = self.getnfreqdoms(devid)
            if resetfreq:
                for freqid in range(0, nfreqdoms):
                    fp = self.getfreqprops(devid, freqid)
                    #print("devid%d/freqid%d: onsubdev=%d, subdevid=%d, canctrl=%d, min_MHz=%d, max_MHz=%d" % (devid, freqid, fp.onsubdev, fp.subdevid, fp.canctrl, fp.min_MHz, fp.max_MHz))
                    self.setfreqlims(devid, freqid, fp.min_MHz, fp.max_MHz)
                    #flims = self.getfreqlims(devid, freqid)
                    #print("devid%d/freqid%d: current min_MHz=%d, max_MHz=%d" % (devid, freqid, flims.min_MHz, flims.max_MHz))
            if resetpwr:
                for pwrid in range(0, npwrdoms):
                    pwrprops = self.getpwrprops(devid, pwrid)
                    self.setpwrlim(devid, pwrid, pwrprops.deflim_mw)
                    curpwrlim_mw = self.getpwrlim(devid, pwrid)
                    # print("devid%d/pwrdid%d: deflim_mw=%d curpwrlim_mw=%d" % (devid, pwrid, pwrprops.deflim_mw, curpwrlim_mw))

                
def basictest(pm, ndevs):
    print("Basic tests")
    for devid in range(0, ndevs):
        npwrdoms = pm.getnpwrdoms(devid)
        nfreqdoms = pm.getnfreqdoms(devid)
        ntempsensors = pm.getntempsensors(devid)
        print("devid=%d: npwrdoms=%d nfreqdoms=%d ntempsensors=%d" % (devid, npwrdoms, nfreqdoms, ntempsensors))

        fspstr = " "*9 # just for formatting
        for pwrid in range(0, npwrdoms):
            pwrprops = pm.getpwrprops(devid, pwrid)
            if pwrprops.canctrl > 0:
                curpwrlim_mw = pm.getpwrlim(devid, pwrid)
                print("%spwrid=%d: deflim_mw=%d curpwrlim_mw=%d" % (fspstr, pwrid, pwrprops.deflim_mw, curpwrlim_mw))
                target_mw = curpwrlim_mw / 4 * 3
                pm.setpwrlim(devid, pwrid, target_mw)
                tmppwrlim_mw = pm.getpwrlim(devid, pwrid)

                if target_mw == tmppwrlim_mw:
                    print("%stesting powercap: passed" % fspstr)
                else:
                    print("%stesting powercap: failed" % fspstr)
                pm.setpwrlim(devid, pwrid, pwrprops.deflim_mw)
        #
        for freqid in range(0, nfreqdoms):
            fp = pm.getfreqprops(devid, freqid)
            print("%sfreqid=%d: onsubdev=%d, subdevid=%d, canctrl=%d, min_MHz=%d, max_MHz=%d" % (fspstr, freqid, fp.onsubdev, fp.subdevid, fp.canctrl, fp.min_MHz, fp.max_MHz))
            if fp.canctrl:
                pm.setfreqlims(devid, freqid, fp.min_MHz+100, fp.max_MHz-100)
                flims = pm.getfreqlims(devid, freqid)
                if (flims.min_MHz == fp.min_MHz+100) and (flims.max_MHz == fp.max_MHz-100):
                    print("%stesting freq. change: passed" % fspstr)
                else:
                    print("%stesting freq. change: failed" % fspstr)
                pm.setfreqlims(devid, freqid, fp.min_MHz, fp.max_MHz)
            print("%scurrent freq=%.1lf MHz" % (fspstr, pm.readfreq(devid,freqid)))
        #
        for tempid in range(0, ntempsensors):
            fp = pm.gettempprops(devid, tempid)
            print("%stempid=%d: onsubdev=%d, subdevid=%d, type=%d currtemp=%.1lf C" % (fspstr, tempid, fp.onsubdev, fp.subdevid, fp.sensortype, pm.readtemp(devid,tempid)))


    print("")

if __name__ == '__main__':
    pm = clr_apmidg()
    ndevs = pm.getndevs()

    basictest(pm, ndevs)
