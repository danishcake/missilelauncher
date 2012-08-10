# Debug:   Before running ensure boost_python-vc80-mt-gd-1_48.dll is in working directory! See boost/stage/libs folder
# Release: Before running ensure boost_python-vc80-mt-1_48.dll    is in working directory! See boost/stage/libs folder

#Import USB Missile Launcher Control library
#Provides just the three functions used below
#and the Action enum
from libmissile import *

InitialiseUSBControl();
PerformAction(Action.RotateLeft);
ShutdownUSBControl();
