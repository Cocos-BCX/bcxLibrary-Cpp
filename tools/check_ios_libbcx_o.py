#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import shutil
import argparse

CUR_DIR = os.path.dirname(os.path.realpath(__file__))

def run(cmd, cwd=None, quiet=False):
    print(cmd, cwd)

    cmds = cmd    # cmd.split(' ')
    if quiet:
        p = subprocess.Popen(cmds, cwd=cwd, shell=True, stdout=subprocess.PIPE)
    else:
        p = subprocess.Popen(cmds, cwd=cwd, shell=True)
    ret = p.wait()
    return ret
    # if ret != 0:
    #     print "==> Command failed: " + cmd
    #     print "==> Stopping build."
    #     sys.exit(1)

def file_lines(file):
    ret = []
    with open(os.path.join(CUR_DIR, file), 'r') as f:
        for line in f.readlines():
            line = line.strip()
            if len(line):
                ret.append(line)
    return ret

def file_append(file, line):
    with open(file, 'a') as f:
        f.write(line)
        f.write('\n')

def check_o():
    lines = file_lines('ios_libbcx_all_o.txt')
    workspace = os.path.join(CUR_DIR, 'package/ios/lib')
    for line in lines:
        run('cp libbcx.a libbcx.a.bak',cwd=workspace)
        run('ar -d libbcx.a ' + line,cwd=workspace)
        ret = run('cocos compile -p ios -s %s' % (os.path.join(CUR_DIR, 'test/bcx-cocos-cpp')))
        if ret != 0:
            run('cp libbcx.a.bak libbcx.a',cwd=workspace)
            file_append('keep_o.txt', line)
        else:
            file_append('remove_o.txt', line)

if __name__ == '__main__':
    check_o()
