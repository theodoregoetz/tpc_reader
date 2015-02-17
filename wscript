#! /usr/bin/env python2
# encoding: utf-8

top     = '.'
out     = 'build'
VERSION = '0.0.1'
APPNAME = 'tpc_reader'


def options(ctx):

    import os,sys

    ctx.load('compiler_cxx boost')

    ### CONFIGURE options
    cfg_opts = ctx.exec_dict['opt'].get_option_group('configure options')

    cfg_opts.add_option('--debug', dest='debug',
        action='store_true', default=False,
        help='Build in debug mode. default: %default')

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
    bld_opts = ctx.exec_dict['opt'].get_option_group('build and install options')

    bld_opts.add_option('--all', dest='all',
        action='store_true', default=False,
        help='Build everything including unit tests. default: %default')
    bld_opts.add_option('--test', dest='test',
        action='store_true', default=False,
        help='Build unit tests. default: %default')



def configure(ctx):
    import os,re
    from subprocess import Popen, PIPE

    ctx.load('compiler_cxx')

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


    system_libpath = get_system_libpath(ctx.env.CXX)
    system_includes = get_system_includes(ctx.env.CXX)


    if ctx.options.libpath != None:
        for libpath in re.split('[:,]+', ctx.options.libpath):
            if libpath != '':
                if libpath not in system_libpath:
                    ctx.env.append_unique('LIBPATH',libpath)
    if ctx.options.includes != None:
        for incpath in re.split('[:,]+', ctx.options.includes):
            if incpath != '':
                if incpath not in system_includes:
                    ctx.env.append_unique('INCLUDES', incpath)


    ### MANDATORY DEPENDENCIES

    try:
        cxx11_code = '#include <array>\nint main() {}\n'

        cxx11env = ctx.env.derive()
        ctx.parse_flags('-std=c++11', 'C++11', env=cxx11env)
        ctx.check_cxx(fragment=cxx11_code, env=cxx11env, use='C++11', msg='C++11 support')
        ctx.parse_flags('-std=c++11', 'C++11')

    except ctx.errors.ConfigurationError:

        cxx11env = ctx.env.derive()
        ctx.parse_flags('-std=c++0x', 'C++11', env=cxx11env)
        ctx.check_cxx(fragment=cxx11_code, env=cxx11env, use='C++11', msg='C++0x support')
        ctx.parse_flags('-std=c++0x', 'C++11')

    """
    ctx.load('boost')
    ctx.check_boost()

    boost_required_libs = '''
        program_options
        filesystem
        system
        log
        log_setup
        date_time
        chrono
        thread
    '''.split()

    for libname in boost_required_libs:
        libpath,lib = ctx.boost_get_libs(libname)
        if libpath not in system_libpath:
            ctx.env.append_unique('LIBPATH_BOOST',libpath)
        ctx.env.append_unique('LIB_boost_'+libname, lib)
        ctx.to_log('boost library found: {}'.format(libname))

    ctx.env.append_unique('LIB_boost_log',
        ['boost_'+l for l in '''\
            log_setup
            filesystem
            system
            date_time
            chrono
            thread
        '''.split()])
    ctx.env.append_unique('DEFINES', 'HAVE_BOOST_LOG')

    ctx.env.append_unique('LIB_boost_filesystem',['boost_system'])

    ctx.env.append_unique('DEFINES_BOOST', ['BOOST_LOG_DYN_LINK'])

    ctx.check_cxx(
        uselib_store = 'pthread',
        lib          = ['pthread'],
        msg          = 'Checking for pthread')

    ctx.check_cfg(
        uselib_store = 'MYSQL',
        path         = 'mysql_config',
        args         = ['--cflags', '--libs'],
        package      = '' )

    def remove_flags(ctx, package_name, *list_of_flags):
        flagtypes = 'CXXFLAGS CFLAGS LINKFLAGS'.split(' ')
        cenvlist = []
        for ft in flagtypes:
            cenvlist += [ctx.env[ft+'_'+package_name]]
        for f in list_of_flags:
            for cenv in cenvlist:
                if f in cenv:
                    cenv.remove(f)

    remove_flags(ctx, 'MYSQL', '-fstack-protector-strong')
    """


    ### OPTIONAL DEPENDENCIES

    """
    boost_optional_libs = '''
        unit_test_framework
    '''.split()
    for libname in boost_optional_libs:
        try:
            libpath,lib = ctx.boost_get_libs(libname)
            if libpath not in system_libpath:
                ctx.env.append_unique('LIBPATH_BOOST',libpath)
            ctx.env.append_unique('LIB_boost_'+libname, lib)
            ctx.to_log('boost library found: {}'.format(libname))
        except ctx.errors.ConfigurationError:
            ctx.to_log('boost library not found: {}'.format(libname))

    try:
        ctx.check_cxx(
            uselib_store = 'EXPAT',
            lib          = 'expat',
            header_name  = ['expat.h'],
            msg          = 'Checking for expat')
    except ctx.errors.ConfigurationError:
        ctx.check_cfg(
            uselib_store = 'EXPAT',
            package      = 'expat',
            args         = ['--cflags', '--libs'],
            mandatory    = False )

    ### some configuration based on the options above
    if not ctx.options.debug:
        ctx.env.append_unique('DEFINES_BOOST', ['NDEBUG'])

    ### recurse into external dependencies that are included
    ### with this project's source tree
    ctx.recurse('ext')
    """


def build(ctx):
    #ctx.recurse('ext')
    #ctx.recurse('src')
    #ctx.recurse('test')
    ctx.recurse('tools')
