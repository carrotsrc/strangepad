#!/bin/python

import sys
import os
import math

if len(sys.argv) < 2:
	print("Usage:\n\tfilter_folpf.py fc(Hz) [fs(Hz)]")
	exit()
fs = 44100
fc = sys.argv[1]

if len(sys.argv) > 2:
    fs = sys.argv[2]

theta = 2.0 * math.pi * float(fc)/float(fs)
gp = math.cos(theta)
gamma = 2.0 - gp
b1 = math.sqrt(math.pow(gamma,2)-1) - gamma
a0 = 1 + b1


print("{}Hz ({}Hz)".format(fc, fs))
print("a0: {}".format(round(a0,4)))
print("b1: {}".format(round(b1,4)))

print("\no: {}".format(theta))
print("p: {}".format(gp))
print("v: {}".format(gamma))
