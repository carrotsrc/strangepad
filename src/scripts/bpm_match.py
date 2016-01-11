#!/usr/bin/env python2
#encoding: UTF-8

# To change this license header, choose License Headers in Project Properties.
# To change this template file, choose Tools | Templates
# and open the template in the editor.
import sys
import re
from os import environ, getcwd, path, listdir



tag_path = ""
bpm = 80

def print_usage():
    print("Usage:")
    print("bpm_match.py BPM")

# Print an error message with red highlight
def print_error(emsg):
    print("\033[1;31mError\033[1;m: " + emsg)
    print("")
    print_usage()
    exit()

def list_files():
    global tag_path
    return [f for f in listdir(tag_path) if path.isfile(path.join(tag_path, f))]

def is_match(lines, bpm):
    title = "-"
    artist = "-"
    album = "-"
    tbpm = "-"
    diff = 0.0
    match = False
    
    for l in lines:
        atom = l.split("=");
        if len(atom) < 2:
            continue;

        if atom[0] == "bpm":

            diff = round((float(atom[1])/float(bpm)), 3)
            if diff < 0.92 or diff > 1.08:
                continue
            match = True
            tbpm = atom[1]

        elif atom[0] == "title":
            title = atom[1]

        elif atom[0] == "artist":
            artist = atom[1]

        elif atom[0] == "album":
            album = atom[1]

    return (match, title, artist, tbpm, diff)

def entry():
    global tag_path, bpm
    tag_path = environ["STRANGEPADFW"] +"/bin/.tags"
    if len(sys.argv) < 2:
        print_error("No BPM specified")

    bpm = sys.argv[1]
    files = list_files()
    for f in files:
        lines = load_file(f)
        (match, title, artist,tbpm, diff)= is_match(lines, int(bpm))
        if match == True:
            print(title + " -- " + artist + "\n\t[{} : {}]\n".format(tbpm, diff))
        
    
    
# Load a StrangeIO tag file with hash
def load_file(hash_file):
    global tag_path
    hf = tag_path+"/"+hash_file

    if not path.isfile(hf):
        print_error("File `"+hash_file+"` does not exist in current directory")

    with open(hf) as f:
        lines = [line.rstrip('\n') for line in f.readlines()]
    return lines

if __name__ == "__main__":
    entry()