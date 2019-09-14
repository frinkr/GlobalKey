#!/usr/bin/env python

import platform
import os
import sys
import subprocess
import shutil

source_root = os.path.dirname(os.path.realpath(__file__))
build_root = os.path.join(source_root, "build")

if len(sys.argv) == 2 and sys.argv[1] == 'clean' and os.path.exists(build_root):
    print('Removing ' + build_root + "...")
    shutil.rmtree(build_root)

if not os.path.exists(build_root):
    os.makedirs(build_root)

def gen_win():
    os.chdir(build_root)
    os.system('cmake -G "Visual Studio 16 2019" -A x64 ' + source_root)

def gen_mac():
    os.chdir(build_root)
    os.system('cmake -G Xcode ' + source_root)

def gen_linux():
    os.chdir(build_root)
    os.system('cmake ' + source_root)

gs = {
    'Linux': gen_linux,
    'Windows': gen_win,
    'Darwin': gen_mac,
}

gs[platform.system()]()

