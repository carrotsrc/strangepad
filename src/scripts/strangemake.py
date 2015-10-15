#!/bin/env python

import sys
import os
from subprocess import call


if not 'STRANGEFW' in os.environ:
	print("Error: environment STRANGEFW does not exist\n\n\tTry 'export STRANGEFW=path/to/framework'")
	exit()
if not 'STRANGEPADFW' in os.environ:
	print("Error: environment STRANGEPADFW does not exist\n\n\tTry 'export STRANGEPADFW=path/to/framework'")
	exit()

fw = os.environ['STRANGEFW']
sw = os.environ['STRANGEPADFW']
owd = os.getcwd();

target_order = [
	'framework',

	'unit/SuAlsa',
	'unit/SuFlac',
	'unit/SuMixer',
	'unit/SuDelay',

	'strangeui',
	'pad/SpFlac',
	'pad/SpMixer',
	'pad/SpSine',
	'pad/SpSine',

	'strangepad',
]	

targets = {
	'framework' :{
			'cd' : fw+"/build/",
			'cmd' : ['make']
			},

	'strangepad' :{
			'cd' : sw+"/src/pad/",
			'cmd' : ['make']
			},

	'strangeui' :{
			'cd' : sw+"/src/panels/ui/",
			'cmd' : ['make']
			},

	'unit/SuAlsa' :{
			'cd' : sw+"/src/units/",
			'cmd' : ['unitbuild', '../../bin/units/SuAlsa', '-O', '-O3', '-lasound', 'strange/SuAlsa.cpp']
			},

	'unit/SuFlac' :{
			'cd' : sw+"/src/units/",
#			'cmd' : ['unitbuild', '../../bin/units/SuFlac', '-O', '-O3', '-lsndfile', 'strange/SuFlac.cpp']
			'cmd' : ['unitbuild', '../../bin/units/SuFlac', '-g', '-lsndfile', 'strange/SuFlac.cpp']
			},

	'unit/SuMixer' :{
			'cd' : sw+"/src/units/",
			'cmd' : ['unitbuild', '../../bin/units/SuMixer',  '-O', '-O3', 'strange/SuMixer.cpp']
			#'cmd' : ['unitbuild', '../../bin/units/SuMixer', '-g', 'strange/SuMixer.cpp']
			},

	'unit/SuDelay' :{
			'cd' : sw+"/src/units/",
			'cmd' : ['unitbuild', '../../bin/units/SuDelay', '-g', 'strange/SuDelay.cpp']
			#'cmd' : ['unitbuild', '../../bin/units/SuDelay', '-O', '-O3', 'strange/SuDelay.cpp']
			},

	'unit/SuEncoder' :{
			'cd' : sw+"/src/units/",
			'cmd' : ['unitbuild', '../../bin/units/SuEncoder', '-g', 'SuEncoder.cpp']
			},

	'unit/RuSine' :{
			'cd' : sw+"/src/units/",
			'cmd' : ['unitbuild', '../../bin/units/RuSine', '-g', 'RuSine.cpp']
			},

#	'unit/BfForderFw' :{
#			'cd' : sw+"/src/units/basic",
#			'cmd' : ['unitbuild', '../../../bin/units/basic/BfForderFw', '-g', 'BfForderFw.cpp']
#			},
#	'unit/BfSineOsc' :{
#			'cd' : sw+"/src/units/basic",
#			'cmd' : ['unitbuild', '../../../bin/units/basic/BfSineOsc', '-g', 'BfSineOsc.cpp']
#			},

#	'unit/BfForderBw' :{
#			'cd' : sw+"/src/units/basic",
#			'cmd' : ['unitbuild', '../../../bin/units/basic/BfForderBw', '-g', 'BfForderBw.cpp']
#			},


	'pad/SpFlac' :{
			'cd' : sw+"/src/panels/SpFlac/",
			'cmd' : ['make']
			},

	'pad/SpMixer' :{
			'cd' : sw+"/src/panels/SpMixer/",
			'cmd' : ['make']
			},

	'pad/SpSine' :{
			'cd' : sw+"/src/panels/SpSine/",
			'cmd' : ['make']
			},

	'pad/SpSine' :{
			'cd' : sw+"/src/panels/SpDelay/",
			'cmd' : ['make']
			},
}

if len(sys.argv) < 2:
	print("Usage:\n\tstrangemake TARGET")
	exit()

if sys.argv[1] == "-v" or sys.argv[1] == "--version":
	print("StrangeMaker build script\nv1.0")
	exit()

if sys.argv[1] == "-a" or sys.argv[1] == "--all":
	print("Running through all targets...")
	for target in target_order:
		print("SMake: \033[1;34m"+target+"\033[1;m")
		os.chdir(targets[target]['cd'])		
		args = targets[target]['cmd'] + sys.argv[2:]
		if call(args) != 0:
			print("Target: \033[1;31mFailed\033[1;m")
			exit()
		print("Target: \033[1;32mOk\033[1;m")
	print("\033[1;32mFinished!\033[1;m")
	exit()


if not sys.argv[1] in targets:
	print("Error - target not found")
	exit()

os.chdir(targets[sys.argv[1]]['cd'])


print("Building target...")

args = targets[sys.argv[1]]['cmd'] + sys.argv[2:]
if "VERBOSE" in os.environ and os.environ['VERBOSE'] == "1":
    print(" ".join(args))

if call(args) == 0:
    print("OK");

os.chdir(owd)
