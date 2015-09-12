#!/bin/env python

import sys
import os
from subprocess import call


if not os.environ.has_key('STRANGEFW'):
	print "Error: environment STRANGEFW does not exist\n\n\tTry 'export STRANGEFW=path/to/framework'"
	exit()
if not os.environ.has_key('STRANGEPADFW'):
	print "Error: environment STRANGEPADFW does not exist\n\n\tTry 'export STRANGEPADFW=path/to/framework'"
	exit()
fw = os.environ['STRANGEFW']
sw = os.environ['STRANGEPADFW']
owd = os.getcwd();



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
			},

	'unit/SuEncoder' :{
			'cd' : sw+"/src/units/",
			'cmd' : ['unitbuild', '../../bin/units/SuEncoder', '-g', 'SuEncoder.cpp']
			},

	'unit/RuSine' :{
			'cd' : sw+"/src/units/",
			'cmd' : ['unitbuild', '../../bin/units/RuSine', '-g', 'RuSine.cpp']
			},

	'unit/BfForderFw' :{
			'cd' : sw+"/src/units/basic",
			'cmd' : ['unitbuild', '../../../bin/units/basic/BfForderFw', '-g', 'BfForderFw.cpp']
			},
	'unit/BfSineOsc' :{
			'cd' : sw+"/src/units/basic",
			'cmd' : ['unitbuild', '../../../bin/units/basic/BfSineOsc', '-g', 'BfSineOsc.cpp']
			},
	'unit/BfForderBw' :{
			'cd' : sw+"/src/units/basic",
			'cmd' : ['unitbuild', '../../../bin/units/basic/BfForderBw', '-g', 'BfForderBw.cpp']
			},
}

if len(sys.argv) < 2:
	print("Usage:\n\tstrangemake TARGET")
	exit()

if sys.argv[1] == "-v" or sys.argv[1] == "--version":
	print("StrangeMaker build script\nv1.0")
	exit()

if not targets.has_key(sys.argv[1]):
	print("Error - target not found")
	exit()

os.chdir(targets[sys.argv[1]]['cd'])


print("Building target...")

args = targets[sys.argv[1]]['cmd'] + sys.argv[2:]
if os.environ.has_key("VERBOSE") and os.environ['VERBOSE'] == "1":
    print(" ".join(args))

if call(args) == 0:
    print("OK");

os.chdir(owd)
