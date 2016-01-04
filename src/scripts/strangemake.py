#!/bin/env python

import sys
import os
import time;
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

build_profile="release"



target_order = [
	'framework',

        'unit/BuLPF',
	'unit/BuPing',
	'unit/BuDump',

	'unit/SuAlsa',
	'unit/SuFlac',
	'unit/SuMixer',
	'unit/SuDelay',

        'unit/SuResample',

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
			'release' : ['make'],
                        'debug' : ['make']
			},

	'strangepad' :{
			'cd' : sw+"/src/pad/",
			'release' : ['make'],
                        'debug' : ['make']
			},

	'strangeui' :{
			'cd' : sw+"/src/panels/ui/",
			'release' : ['make'],
                        'debug' : ['make']
			},

	'unit/SuAlsa' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/SuAlsa', '-O', '-O3', '-lasound', 'strange/SuAlsa.cpp'],
                        'debug' : ['unitbuild', '../../bin/units/SuAlsa', '-g', '-lasound', 'strange/SuAlsa.cpp']
			},

	'unit/SuFlac' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/SuFlac', '-O', '-O3', '-lsndfile', 'strange/SuFlac.cpp'],
			'debug' : ['unitbuild', '../../bin/units/SuFlac', '-g', '-lsndfile', 'strange/SuFlac.cpp']
			},

	'unit/SuMixer' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/SuMixer',  '-O', '-O3', 'strange/SuMixer.cpp'],
			'debug' : ['unitbuild', '../../bin/units/SuMixer', '-g', 'strange/SuMixer.cpp']
			},

	'unit/SuDelay' :{
			'cd' : sw+"/src/units/",
			'debug' : ['unitbuild', '../../bin/units/SuDelay', '-O', '-O3', 'strange/SuDelay.cpp'],
                        'release' : ['unitbuild', '../../bin/units/SuDelay', '-g', 'strange/SuDelay.cpp']
			},

	'unit/SuEncoder' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/SuEncoder', '-O', '-O3', 'SuEncoder.cpp'],
                        'debug' : ['unitbuild', '../../bin/units/SuEncoder', '-g', 'SuEncoder.cpp']
			},

	'unit/SuResample' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/SuResample', '-O', '-O3', '-lsamplerate', 'strange/SuResample.cpp'],
                        'debug' : ['unitbuild', '../../bin/units/SuResample', '-g', '-lsamplerate', 'strange/SuResample.cpp']
			},

	'unit/RuSine' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/RuSine', '-O', '-O3', 'RuSine.cpp'],
                        'debug' : ['unitbuild', '../../bin/units/RuSine', '-g', 'RuSine.cpp']
			},

        'unit/BuLPF' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/BuLPF', '-O', '-O3', 'basic/BuLPF.cpp'],
                        'debug' : ['unitbuild', '../../bin/units/BuLPF', '-g', 'basic/BuLPF.cpp']
			},

        'unit/BuPing' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/BuPing', '-O', '-O3', 'basic/BuPing.cpp'],
                        'debug' : ['unitbuild', '../../bin/units/BuPing', '-g', 'basic/BuPing.cpp']
			},

        'unit/BuDump' :{
			'cd' : sw+"/src/units/",
			'release' : ['unitbuild', '../../bin/units/BuDump', '-O', '-O3', 'basic/BuDump.cpp'],
                        'debug' : ['unitbuild', '../../bin/units/BuDump', '-g', 'basic/BuDump.cpp']
			},

        'pad/SpFlac' :{
			'cd' : sw+"/src/panels/SpFlac/",
			'release' : ['make'],
                        'debug' : ['make'],
			},

	'pad/SpMixer' :{
			'cd' : sw+"/src/panels/SpMixer/",
			'release' : ['make'],
                        'debug' : ['make'],
			},

	'pad/SpSine' :{
			'cd' : sw+"/src/panels/SpSine/",
			'release' : ['make'],
                        'debug' : ['make'],
			},

	'pad/SpSine' :{
			'cd' : sw+"/src/panels/SpDelay/",
			'release' : ['make'],
                        'debug' : ['make'],
			},

}

if len(sys.argv) < 2:
	print("Usage:\n\tstrangemake TARGET")
	exit()

if sys.argv[1] == "-v" or sys.argv[1] == "--version":
	print("StrangeMaker build script\nv1.0")
	exit()

if "--release" in sys.argv:
    build_profile = "release"
    sys.argv.remove("--release")

if "--debug" in sys.argv:
    build_profile = "debug"
    os.putenv('DEBUG_BUILD', "{}".format(1))
    sys.argv.remove("--debug")

print("Build profile: \033[1;35m"+build_profile+"\033[1;m")

tps = time.time();

if sys.argv[1] == "-a" or sys.argv[1] == "--all":
	print("Running through all targets...")
	for target in target_order:

		print("SMake: \033[1;34m"+target+"\033[1;m")
		os.chdir(targets[target]['cd'])		
		args = targets[target][build_profile] + sys.argv[2:]
		if call(args) != 0:
			print("Target: \033[1;31mFailed\033[1;m")
                        total = round(time.time()-tps, 2);
                        print("Duration: {} seconds".format(total));
			exit()
		print("Target: \033[1;32mOk\033[1;m")

	print("\033[1;32mFinished!\033[1;m")
        total = round(time.time()-tps, 2);
        print("Duration: {} seconds".format(total));

	exit()


if not sys.argv[1] in targets:
	print("Error - target not found")
	exit()

os.chdir(targets[sys.argv[1]]['cd'])


print("Building target...")

args = targets[sys.argv[1]][build_profile] + sys.argv[2:]
if "VERBOSE" in os.environ and os.environ['VERBOSE'] == "1":
    print(" ".join(args))

if call(args) == 0:
    print("\033[1;32mFinished!\033[1;m")
else:
    print("Target: \033[1;31mFailed\033[1;m")

total = round(time.time()-tps, 2);
print("Duration: {} seconds".format(total));

os.chdir(owd)
