#!/usr/bin/env python

from distutils.core import setup
#from distutils.command.install import install as DistInstall
#import subprocess

#class CustomInstall(DistInstall):
#    def run(self):
#        subprocess.run(["make"])
#        DistInstall.run(self)

setup(name='pyapmidg',
      version='0.2',
      author='Kazutomo Yoshii',
      author_email='kazutomo@mcs.anl.gov',
#      package_data={'pyapmidg':["libapmidg.so"]},
      packages=['pyapmidg'],
#      cmdclass={'install':CustomInstall}
      )
