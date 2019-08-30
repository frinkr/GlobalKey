#!/usr/bin/env python

import platform
import os
import sys
import subprocess
import shutil

source_root = os.path.dirname(os.path.realpath(__file__))
build_root = os.path.join(source_root, "build")

if len(sys.argv) == 2 and sys.argv[1] == 'clean' and os.path.exists(build_root):
    print('removing ' + build_root + "...")
    shutil.rmtree(build_root)

if not os.path.exists(build_root):
    os.makedirs(build_root)

def gen_win():
    vcpkg_home = os.environ['VCPKG_HOME'] if 'VCPKG_HOME' in os.environ else None
    if vcpkg_home == None or len(vcpkg_home) == 0:
        raise OSError('Environment variable VCPKG_HOME is not defined')
    os.chdir(build_root)
    os.system('cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_TOOLCHAIN_FILE=' + os.path.join(vcpkg_home, 'scripts/buildsystems/vcpkg.cmake ') + source_root)

def gen_mac():
    openssl = subprocess.check_output('brew --prefix openssl', shell=True).decode('utf-8').strip()
    qt = subprocess.check_output('brew --prefix qt', shell=True).decode('utf-8').strip()
    os.environ['PKG_CONFIG_PATH'] = os.path.join(openssl, 'lib/pkgconfig')
    os.chdir(build_root)
    os.system('cmake -G Xcode -DCMAKE_PREFIX_PATH=' + qt + ' ' + source_root)

def gen_linux():
    os.chdir(build_root)
    os.system('cmake ' + source_root)

gs = {
    'Linux': gen_linux,
    'Windows': gen_win,
    'Darwin': gen_mac,
}

gs[platform.system()]()

