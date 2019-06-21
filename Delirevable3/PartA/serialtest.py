#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import serial

DEVPATH = "/dev"
TTYPREFIX = "ttyACM"
TESTSTRING = b"Hello Test World!"

if __name__=='__main__':
  for tty in (os.path.join(DEVPATH,tty) for tty in os.listdir(DEVPATH) \
                                                if tty.startswith(TTYPREFIX)):
    try:
      ctt = serial.Serial(tty, timeout=1, writeTimeout=1)
    except serial.SerialException:
      continue
    ctt.flushInput()
    ctt.flushOutput()
#    print(ctt)
    try:
      ctt.write(TESTSTRING)
    except serial.SerialTimeoutException:
      ctt.__exit__()
      continue
    for retry in range(3): # Try three times to read connection test result
      ret = ctt.read(2*len(TESTSTRING))
#      print("ret: " + repr(ret))
      if TESTSTRING in ret:
        sys.exit(0)
        break
    else:
      ctt.__exit__()
      continue
    break
  else:
    print("Failed")
    sys.exit(1)
