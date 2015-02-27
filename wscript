#! /usr/bin/env python2
# encoding: utf-8

import os,sys,re
from subprocess import Popen, PIPE
from distutils.spawn import find_executable as which

top     = '.'
out     = 'build'
VERSION = '0.1.1'
APPNAME = 'tpc_reader'

def options(opt):

    opt.load('compiler_cxx')

    ### CONFIGURE options
    cfg_opts = opt.exec_dict['opt'].get_option_group('configure options')

    cfg_opts.add_option('--debug', dest='debug',
        action='store_true', default=False,
        help='Build in debug mode. default: %default')

    cfg_opts.add_option('--no-opt', dest='no_opt',
        action='store_true', default=False,
        help='Turn off optimizations. default: %default')

    cfg_opts.add_option('--includes', dest='includes',
        default = None,
        help = 'list of include paths applied to all targets, separated by colons (:) or commas (,). default: %default')

    if 'win32' in sys.platform:
        ldlibpathenv = 'PATH'
    elif 'darwin' in sys.platform:
        ldlibpathenv = 'DYLD_LIBRARY_PATH'
    else: # linux-like
        ldlibpathenv = 'LD_LIBRARY_PATH'

    default_libpath = os.environ.get(ldlibpathenv,None)
    cfg_opts.add_option('--libpath', dest='libpath',
        default = default_libpath,
        help = 'list of include paths applied to all targets, separated by colons (:) or commas (,). default: %default')

    ### BUILD options
    bld_opts = opt.exec_dict['opt'].get_option_group('build and install options')

    bld_opts.add_option('--tests', dest='build_tests',
        action='store_true', default=False,
        help='Build unit tests and other testing programs. default: %default')
    bld_opts.add_option('--all', dest='build_all',
        action='store_true', default=False,
        help='Build everything including unit tests. default: %default')

    opt.recurse('ext')


def configure(conf):

    conf.load('compiler_cxx')

    def get_system_libpath(compiler):
        pathlist = []

        if compiler in ['gcc', 'g++', 'clang', 'clang++']:

            output = Popen(compiler + ' -print-search-dirs',
                shell=True,
                env=os.environ,
                executable=os.environ.get('SHELL', '/bin/bash'),
                stdout=PIPE,
                stderr=PIPE).communicate()[0]

            for line in output.split('\n'):
                if line.startswith('libraries:'):
                    for p in line.split('=')[-1].split(':'):
                        pathlist.append(os.path.normpath(p))

        return pathlist

    def get_system_includes(compiler):
        pathlist = []

        if compiler in ['gcc', 'g++', 'clang', 'clang++']:

            preprocessor = Popen(compiler + ' -print-prog-name=cpp',
                shell=True,
                env=os.environ,
                executable=os.environ.get('SHELL', '/bin/bash'),
                stdout=PIPE,
                stderr=PIPE).communicate()[0].split('\n')[0]

            output = Popen(preprocessor + ' -v',
                shell=True,
                env=os.environ,
                executable=os.environ.get('SHELL', '/bin/bash'),
                stdout=PIPE,
                stderr=PIPE,
                stdin=PIPE).communicate(input='')[1].split('\n')

            in_search_list = False
            for line in output:
                if line.startswith('#include') and line.endswith('search starts here:'):
                    in_search_list = True
                elif line.startswith('End of search list'):
                    in_search_list = False
                elif in_search_list:
                    pathlist.append(os.path.normpath(line))

        return pathlist


    system_libpath = get_system_libpath(conf.env.CXX)
    system_includes = get_system_includes(conf.env.CXX)


    if conf.options.libpath != None:
        for libpath in re.split('[:,]+', conf.options.libpath):
            if libpath != '':
                if libpath not in system_libpath:
                    conf.env.append_unique('LIBPATH',libpath)
    if conf.options.includes != None:
        for incpath in re.split('[:,]+', conf.options.includes):
            if incpath != '':
                if incpath not in system_includes:
                    conf.env.append_unique('INCLUDES', incpath)


    ### MANDATORY DEPENDENCIES

    cxx11_code = '#include <array>\nint main() {}\n'

    try:

        cxx11env = conf.env.derive()
        conf.parse_flags('-std=c++11', 'C++11',
            env=cxx11env)
        conf.check_cxx(fragment=cxx11_code, env=cxx11env,
            use='C++11', msg='C++11 support')
        conf.parse_flags('-std=c++11', 'C++11')
        conf.env['HAS_C++11'] = True

    except conf.errors.ConfigurationError:

        cxx11env = conf.env.derive()
        conf.parse_flags('-std=c++0x', 'C++11',
            env=cxx11env)
        conf.check_cxx(fragment=cxx11_code, env=cxx11env,
            use='C++11', msg='C++0x support')
        conf.parse_flags('-std=c++0x', 'C++11')
        conf.env['HAS_C++11'] = True

    except conf.errors.ConfigurationError:

        conf.env['HAS_C++11'] = False
        print '''\
C++11 may not be supported by your compiler.

This will disable certain programs / features
in this project, but the main parts should
still work.

Please consider upgrading your C++ compiler.
'''

    if not conf.options.no_opt:
        try:
            conf.check_cxx(
                uselib_store = 'OPT',
                cxxflags = ['-Ofast'])
            conf.env.CXXFLAGS += ['-Ofast']
        except conf.errors.ConfigurationError:
            conf.env.CXXFLAGS += ['-O3']

    if conf.options.debug:
        conf.env.CXXFLAGS += ['-g','-DDEBUG']


    conf.check_cfg(
        uselib_store = 'ROOT',
        path         = 'root-config',
        args         = ['--cflags', '--glibs'],
        package      = '' )

    conf.check_cxx(
        uselib_store = 'pthread',
        lib          = ['pthread'],
        msg          = 'Checking for pthread')

    conf.recurse('ext')

    """
    try:
        conf.check_cxx(
            uselib_store = 'EXPAT',
            lib          = 'expat',
            header_name  = ['expat.h'],
            msg          = 'Checking for expat')
    except conf.errors.ConfigurationError:
        conf.check_cfg(
            uselib_store = 'EXPAT',
            package      = 'expat',
            args         = ['--cflags', '--libs'],
            mandatory    = False )

    """


def build(bld):

    if bld.options.build_all:
        build_tools = True
        bld.options.build_tests = True
    elif bld.options.build_tests:
        build_tools = False
    else:
        build_tools = True

    bld.recurse('ext')

    if build_tools:
        bld.recurse('tools')

    if bld.options.build_tests:
        bld.recurse('test')
