#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import shutil
import argparse
import platform

CUR_DIR = os.path.dirname(os.path.realpath(__file__))

# move to config.json ?
boost_libs = ['thread', 'date_time', 'system', 'filesystem', 'chrono']
boost_android_headers = [
'array.hpp',
'assert.hpp',
'call_traits.hpp',
'concept',
'concept_check',
'concept_check.hpp',
'config',
'config.hpp',
'container',
'core',
'cstdint.hpp',
'current_function.hpp',
'detail',
'exception',
'exception_ptr.hpp',
'integer',
'integer.hpp',
'integer_fwd.hpp',
'integer_traits.hpp',
'interprocess',
'intrusive',
'iterator',
'iterator.hpp',
'lexical_cast',
'lexical_cast.hpp',
'limits.hpp',
'math',
'move',
'mpl',
'multi_index',
'multi_index_container_fwd.hpp',
'next_prior.hpp',
'noncopyable.hpp',
'numeric',
'operators.hpp',
'predef',
'predef.h',
'preprocessor',
'range',
'range.hpp',
'rational.hpp',
'static_assert.hpp',
'swap.hpp',
'throw_exception.hpp',
'type.hpp',
'type_traits',
'utility',
'utility.hpp',
'version.hpp',
]

def rm(file):
    if os.path.exists(file):
        os.remove(file)

def run(cmd, cwd=None, quiet=False,exit=True):
    print(cmd, cwd)

    cmds = cmd    # cmd.split(' ')
    if quiet:
        p = subprocess.Popen(cmds, cwd=cwd, shell=True, stdout=subprocess.PIPE)
    else:
        p = subprocess.Popen(cmds, cwd=cwd, shell=True)
    ret = p.wait()
    if ret != 0:
        print "==> Command failed: " + cmd
        print "==> Stopping build."
        if exit:
            sys.exit(1)
    return ret

def makeSureDirs(path):
    if os.path.exists(path):
        return
    #makeSureDirs(os.path.dirname(path))
    os.makedirs(path)

def unicopy(src, dst):
    src = os.path.realpath(src)
    dst = os.path.realpath(dst)
    if not os.path.exists(src):
        print('unicopy %s source is not exist' % (src))
        return
    makeSureDirs(os.path.dirname(dst))
    if os.path.isfile(src):
        shutil.copy(src, dst)
    elif os.path.isdir(src):
        if os.path.exists(dst):
            shutil.rmtree(dst)
        shutil.copytree(src, dst)
    else:
        print('Unknow copy file type')

def strip_libbcx_ios(libbcx_static_lib, strip_type):

    if strip_type == 'none': return

    join = os.path.join
    workspace = os.path.dirname(libbcx_static_lib)
    archs = ['i386', 'x86_64', 'armv7', 'arm64']

    objs = []
    if strip_type == 'all':
        objs = file_lines(join(CUR_DIR, 'ios_libbcx_remove_o_all.txt'))
    else:
        objs = file_lines(join(CUR_DIR, 'ios_libbcx_remove_o.txt'))

    for arch in archs:

        rm(join(workspace,arch+'.a'))
        cmd = 'lipo {liba} -thin {arch} -output {arch}.a '.format(liba = libbcx_static_lib, arch = arch)
        run(cmd, cwd=workspace)

        for o in objs:
            run('ar -d {arch}.a {o}'.format(arch=arch,o=o),cwd=workspace,exit=False)

        # run('rm -fr *.o', cwd=workspace)
        # rm(join(workspace,'__.SYMDEF'))

        # run('ar x {arch}.a'.format(arch=arch), cwd=workspace)

        # for o in file_lines(join(CUR_DIR, 'ios_libbcx_remove_o.txt')):
        #     run('rm -fr {o}'.format(o=o),cwd=workspace)

        # rm(join(workspace,arch+'.a'))
        # run('libtool -static -o {arch}.a *.o'.format(arch=arch,libtool=libtool),cwd=workspace)

        # run('rm -fr *.o', cwd=workspace)
        # rm(join(workspace,'__.SYMDEF'))


    rm(libbcx_static_lib)
    run('lipo -create -output {liba} {archs}'.format(liba=libbcx_static_lib,archs=' '.join([arch+'.a' for arch in archs]))
        , cwd=workspace)

    # rm all arch .a
    for arch in archs:
        rm(join(workspace,arch+'.a'))

def compileIOSLibs(proj_path):
    mode_str = 'Release'
    target = 'bcx'
    tmp_folder = os.path.join(CUR_DIR, 'tmp')
    ios_sim_libs_dir = os.path.join(tmp_folder, "simulator")
    ios_dev_libs_dir = os.path.join(tmp_folder, "device")

    XCODE_CMD_FMT = "xcodebuild -project \"%s\" -configuration %s -target \"%s\" %s CONFIGURATION_BUILD_DIR=%s GCC_GENERATE_DEBUGGING_SYMBOLS=NO"

    # compile ios simulator
    build_cmd = XCODE_CMD_FMT % (proj_path, mode_str, target, "-sdk iphonesimulator ARCHS=\"i386 x86_64\" VALID_ARCHS=\"i386 x86_64\"", ios_sim_libs_dir)
    run(build_cmd)

    # compile ios device
    build_cmd = XCODE_CMD_FMT % (proj_path, mode_str, target, "-sdk iphoneos", ios_dev_libs_dir)
    run(build_cmd)

    print('ios_sim_libs_dir', ios_sim_libs_dir)
    print('ios_dev_libs_dir', ios_dev_libs_dir)

    # generate fat libs for iOS
    print('>>>>> generate fat lib for iOS')
    for lib in os.listdir(ios_sim_libs_dir):
        sim_lib = os.path.join(ios_sim_libs_dir, lib)
        dev_lib = os.path.join(ios_dev_libs_dir, lib)
        output_lib = os.path.join(tmp_folder, lib)
        lipo_cmd = "lipo -create -output \"%s\" \"%s\" \"%s\"" % (output_lib, sim_lib, dev_lib)

        run(lipo_cmd)

def compileMacLibs(proj_path):
    mode_str = 'Release'
    target = 'bcx mac'
    tmp_folder = os.path.join(CUR_DIR, 'tmp')
    mac_libs_dir = os.path.join(tmp_folder, "mac")

    XCODE_CMD_FMT = "xcodebuild -project \"%s\" -configuration %s -target \"%s\" %s CONFIGURATION_BUILD_DIR=%s GCC_GENERATE_DEBUGGING_SYMBOLS=NO"

    # compile mac
    build_cmd = XCODE_CMD_FMT % (proj_path, mode_str, target, "", mac_libs_dir)
    run(build_cmd)

def makeFolderEmpty(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    makeSureDirs(path)

def rmTmpIf():
    tmp_folder = os.path.join(CUR_DIR, 'tmp')
    if os.path.exists(tmp_folder):
        shutil.rmtree(os.path.join(CUR_DIR, 'tmp'))

def genIOSLibs():
    ios_project_folder = os.path.join(CUR_DIR, '..', 'bcx', 'project', 'bcx-lib-ios')
    package_ios_folder = os.path.join(CUR_DIR, '..', 'bcx-sdk', 'package', 'ios')

    makeFolderEmpty(os.path.join(CUR_DIR, 'tmp'))

    # run('xcodebuild -arch x86_64', cwd=ios_project_folder)
    compileIOSLibs(os.path.realpath(os.path.join(ios_project_folder, 'bcx.xcodeproj')))

    # copy lib
    src_lib = os.path.join(CUR_DIR, 'tmp', 'libbcx.a')
    if not os.path.exists(src_lib):
        print('ERROR! libbcx.a is not exist')
        exit(1)
    makeSureDirs(os.path.join(package_ios_folder, 'lib'))
    shutil.copy(src_lib, os.path.join(package_ios_folder, 'lib'))
    # ios_lib_folder = os.path.join(ios_project_folder, '..', '..', 'libbcx')

    # copy ios header
    copyIOSXHeader(package_ios_folder)

    makeFolderEmpty(os.path.join(CUR_DIR, 'tmp'))

def copyIOSXHeader(package_path):
    bcx_lib_folder = os.path.join(CUR_DIR, '..', 'bcx', 'libbcx')

    headers = [
        'BCX.hpp'
    ]

    # copy header
    dst_include_folder = os.path.join(package_path, 'include')
    for header in headers:
        dst = os.path.join(dst_include_folder, header)
        makeSureDirs(os.path.dirname(dst))
        unicopy(os.path.join(bcx_lib_folder, header), dst)

    unicopy(os.path.join(bcx_lib_folder, '3rd/boost/ios/include/boost'),
        os.path.join(dst_include_folder, 'boost'))
    unicopy(os.path.join(bcx_lib_folder, '3rd/openssl/include/ios/openssl'),
        os.path.join(dst_include_folder, 'openssl'))

def genMacLibs():
    ios_project_folder = os.path.join(CUR_DIR, '..', 'bcx', 'project', 'bcx-lib-ios')
    package_mac_folder = os.path.join(CUR_DIR, '..', 'bcx-sdk', 'package', 'mac')

    makeFolderEmpty(os.path.join(CUR_DIR, 'tmp', 'mac'))

    # run('xcodebuild -arch x86_64', cwd=ios_project_folder)
    compileMacLibs(os.path.realpath(os.path.join(ios_project_folder, 'bcx.xcodeproj')))

    # copy lib
    src_lib = os.path.join(CUR_DIR, 'tmp', 'mac', 'libbcx mac.a')
    if not os.path.exists(src_lib):
        print('ERROR! libbcx mac.a is not exist')
        exit(1)
    makeSureDirs(os.path.join(package_mac_folder, 'lib'))
    shutil.copy(src_lib, os.path.join(package_mac_folder, 'lib', 'libbcx.a'))

    # copy ios header
    copyIOSXHeader(package_mac_folder)

    makeFolderEmpty(os.path.join(CUR_DIR, 'tmp', 'mac'))

def get_ndk_root():
    ndk_root = os.getenv("NDK_ROOT", "")
    if not ndk_root:
        ndk_root = os.environ.get('ANDROID_NDK_ROOT')
    return ndk_root

def file_lines(file='libbcx.o.txt'):
    ret = []
    with open(os.path.join(CUR_DIR, file), 'r') as f:
        for line in f.readlines():
            line = line.strip()
            if len(line):
                ret.append(line)
    return ret

def strip_static_lib(lib_file, is_arm):
    ndk_root = get_ndk_root()
    if ndk_root:
        strip_path = "arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/arm-linux-androideabi/bin/strip"

        if 'arm64-v8a' in lib_file:
            strip_path = "aarch64-linux-android-4.9/prebuilt/darwin-x86_64/aarch64-linux-android/bin/strip"
        if not is_arm:
            strip_path = "x86-4.9/prebuilt/darwin-x86_64/i686-linux-android/bin/strip"
        strip_path = ndk_root + "/toolchains/" + strip_path
        # print('> Strip path =', strip_path, os.path.exists(strip_path))
        # print('> lib file =', lib_file, os.path.getsize(lib_file))
        run(strip_path + " -S " + lib_file)
        # print('> Strip DONE. File size=', os.path.getsize(lib_file))
    else:
        print('> Can not get ANDROID_NDK_ROOT')

def get_suitbale_abis(path, abis):
    files = os.listdir(path)

    return [val for val in abis if val in files]

def get_suitable_tool(ndk_root, abi, cmd):
    # ./toolchains/x86-4.9/prebuilt/darwin-x86_64/i686-linux-android/bin/ar
    # ./toolchains/x86_64-4.9/prebuilt/darwin-x86_64/x86_64-linux-android/bin/ar
    # ./toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/arm-linux-androideabi/bin/ar
    # ./toolchains/aarch64-linux-android-4.9/prebuilt/darwin-x86_64/aarch64-linux-android/bin/ar

    path = os.path.join(ndk_root, 'toolchains')

    if 'armeabi' == abi:
        path = os.path.join(path, 'arm-linux-androideabi-4.9')
    elif 'armeabi-v7a' == abi:
        path = os.path.join(path, 'arm-linux-androideabi-4.9')
    elif 'arm64-v8a' == abi:
        path = os.path.join(path, 'aarch64-linux-android-4.9')
    elif 'x86' == abi:
        path = os.path.join(path, 'x86-4.9')
    elif 'x86_64' == abi:
        path = os.path.join(path, 'x86_64-4.9')
    else:
        print('WARNING! unknow ABI {}, take as arm'.format(abi))
        path = os.path.join(path, 'arm-linux-androideabi-4.9')

    path = os.path.join(path, 'prebuilt')

    if 'Windows' == platform.system():
        path = os.path.join(path, 'windows-x86_64')
    else:
        path = os.path.join(path, 'darwin-x86_64')

    if 'armeabi' == abi:
        path = os.path.join(path, 'arm-linux-androideabi')
    elif 'armeabi-v7a' == abi:
        path = os.path.join(path, 'arm-linux-androideabi')
    elif 'arm64-v8a' == abi:
        path = os.path.join(path, 'aarch64-linux-android')
    elif 'x86' == abi:
        path = os.path.join(path, 'i686-linux-android')
    elif 'x86_64' == abi:
        path = os.path.join(path, 'x86_64-linux-android')
    else:
        print('WARNING! unknow ABI {}, take as arm'.format(abi))
        path = os.path.join(path, 'arm-linux-androideabi')

    path = os.path.join(path, 'bin')

    path = os.path.join(path, cmd)
    if 'Windows' == platform.system():
        path += ".exe"

    if not os.path.isfile(path):
        print("ERROR! can't find ndk tool {}".format(path))
        sys.exit(1)

    return path

def compileAndroidLibs(sample_proj_path, lib_proj_path, output_path):
    print('>>>>> compile bcx lib for android')
    ndk_root = get_ndk_root()
    if 'Windows' == platform.system():
        build_cmd = ".\gradlew.bat assembleRelease"
    else:
        build_cmd = "./gradlew :bcx-lib:assembleRelease"
    # run(build_cmd, cwd=sample_proj_path)

    mode_str = 'release'
    mode_folder = os.path.join(lib_proj_path, ".cxx", "cmake", mode_str)
    abis = get_suitbale_abis(mode_folder, ["armeabi", "armeabi-v7a", "arm64-v8a", "x86", "x86_64"])

    def find_a_file(folder):
        for f in os.listdir(folder):
            if f.endswith(".a"):
                return os.path.join(folder, f)
        return None

    for abi_str in abis:
        libs_folder = os.path.join(mode_folder, abi_str, "libbcx")

        lib_bcxapi = find_a_file(libs_folder)

        output_lib = os.path.join(output_path, 'prebuilt', 'android', abi_str, 'libbcx.a')
        if os.path.exists(lib_bcxapi):
            print('>>>>> strip bcx lib for android ' + abi_str)
            unicopy(os.path.realpath(lib_bcxapi), output_lib)
            strip_command = get_suitable_tool(ndk_root, abi_str, "strip")
            run("{} -S {}".format(strip_command, output_lib))

def copyAndroidHeader(android_package_root):
    bcx_lib_folder = os.path.join(CUR_DIR, '..', 'bcx', 'libbcx')
    headers = [
        'BCX.hpp'
    ]

    # copy header
    dst_include_folder = os.path.join(android_package_root, 'include')
    for header in headers:
        dst = os.path.join(dst_include_folder, header)
        makeSureDirs(os.path.dirname(dst))
        unicopy(os.path.join(bcx_lib_folder, header), dst)

    # copy 3rd
    unicopy(os.path.join(bcx_lib_folder, '3rd'), os.path.join(os.path.dirname(android_package_root), '3rd'))

def genAndroidLibs():
    # android_lib_project_folder = os.path.realpath(os.path.join(CUR_DIR, '..', 'bcx', 'project', 'bcx-lib-android'))
    android_sample_project_folder = os.path.realpath(os.path.join(CUR_DIR, '..', 'test', 'bcx-android'))
    android_lib_project_folder = os.path.realpath(os.path.join(android_sample_project_folder, 'app'))
    bcx_package_folder = os.path.realpath(os.path.join(CUR_DIR, '..', 'bcx-sdk', 'package', 'bcx'))

    makeFolderEmpty(os.path.join(CUR_DIR, 'tmp'))

    compileAndroidLibs(android_sample_project_folder, android_lib_project_folder, bcx_package_folder)

    copyAndroidHeader(bcx_package_folder)

    makeFolderEmpty(os.path.join(CUR_DIR, 'tmp'))

def genWindowsLibs():
    package_win_folder = os.path.realpath(os.path.join(CUR_DIR, '..', 'bcx-sdk', 'package', 'win'))
    devenv = 'C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\Common7\\IDE\\devenv.exe'
    if not os.path.exists(devenv):
        print("Visual Studio 2017 devenv.exe not find")
        return

    bcx_lib_win_folder = os.path.realpath(os.path.join(CUR_DIR, '..', 'bcx', 'project', 'bcx-lib-win'))
    sln = os.path.realpath(os.path.join(bcx_lib_win_folder, 'bcx-lib-win.sln'))
    vcxproj = os.path.realpath(os.path.join(bcx_lib_win_folder, 'bcx-lib-win.vcxproj'))

    devenv_build_format = '"%s" "%s" /Rebuild "%s|%s" /Project "%s"'

    for mode in ["Release", "Debug"]:
        for arch in ["x86"]:
            cmd = devenv_build_format % (devenv, sln, mode, arch, vcxproj)
            # run(cmd)
            if "x86" == arch:
                bcx_lib = os.path.join(bcx_lib_win_folder, mode, "bcx-lib-win.lib")
            else:
                bcx_lib = os.path.join(bcx_lib_win_folder, "x64", mode, "bcx-lib-win.lib")
            unicopy(bcx_lib, os.path.join(package_win_folder, "prebuilt", mode.lower(), arch, "bcx-lib-win.lib"))

    # copy ios header
    unicopy(os.path.realpath(os.path.join(CUR_DIR, '..', 'bcx-sdk', 'package', 'ios', 'include')),
        os.path.join(package_win_folder, 'include'))

    # remove boost, will use Boost-For-Android's header files
    makeFolderEmpty(os.path.join(package_win_folder, 'include', 'boost'))

    # copy boost headers
    boost_include_dir = os.path.realpath(os.path.join(CUR_DIR, '..', '..', 'boost_1_64_0', 'boost'))
    for header in boost_android_headers:
        f = os.path.join(boost_include_dir, header)
        unicopy(f, os.path.join(package_win_folder, 'include', 'boost', header))

def preEnvCheckNDK():
    ndk_root = get_ndk_root()
    if not ndk_root:
        print('can not find env NDK_ROOT')
        sys.exit(1)

def preEnvCheckWindows():
    boost_root = os.path.realpath(os.path.join(CUR_DIR, '..', '..', 'boost_1_64_0'))
    if not os.path.exists(boost_root):
        print('can not find boost_1_64_0 for windows at path:')
        print(boost_root)
        print('you can download from this link:')
        print('https://sourceforge.net/projects/boost/files/boost-binaries/1.64.0/boost_1_64_0-msvc-14.1-32.exe/download')
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', dest='platform', default='b', choices=['i', 'm', 'a', 'w', 'b'],
                        help='Platfrom: ios(i)/mac(m)/android(a)/win(w)/both(b)(default)')
    parser.add_argument('-c', '--clean', dest='clean', default='y', choices=['y','n'],
                        help='remove temp folder')

    (args, unknown) = parser.parse_known_args()
    if len(unknown) > 0:
        print("unknown arguments: %s" % unknown)
        parser.print_help()
        return

    if args.platform in ['b','i'] and 'Darwin' == platform.system():
        print('>>>>> generate ios libs')
        genIOSLibs()

        # strip unused .o in libbcx.a
        # package_ios_folder = os.path.join(CUR_DIR, '..', 'bcx-sdk', 'package', 'ios')
        # strip_libbcx_ios(os.path.join(package_ios_folder, 'lib', 'libbcx.a'), 'none')

    if args.platform in ['b','m'] and 'Darwin' == platform.system():
        print('>>>>> generate mac libs')
        genMacLibs()

    if args.platform in ['b','a']:
        print('>>>>> generate android libs')
        preEnvCheckNDK()
        genAndroidLibs()

    if args.platform in ['b','w'] and 'Windows' == platform.system():
        print('>>>>> generate windows libs')
        preEnvCheckWindows()
        genWindowsLibs()

    if args.clean == 'y':
        rmTmpIf()


if __name__ == "__main__":
    main()
