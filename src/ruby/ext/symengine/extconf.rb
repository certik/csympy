require 'mkmf'

# Load Libraries
LIBDIR      = RbConfig::CONFIG['libdir']
INCLUDEDIR  = RbConfig::CONFIG['includedir']

HEADER_DIRS = [
    # First search /opt/local for macports
    '/opt/local/include',

    # Then search /usr/local for people that installed from source
    '/usr/local/include',

    # Check the ruby install locations
    INCLUDEDIR,

    '/home/certik/repos/symengine/src',
    '/local/certik/bld/profile/xt33omorophg/include',

    # Finally fall back to /usr
    '/usr/include'
]

LIB_DIRS = [
    # Then search /usr/local for people that installed from source
    '/usr/local/lib',

    # Check the ruby install locations
    LIBDIR,
    '/home/certik/repos/symengine/src',

    # Finally fall back to /usr
    '/usr/lib'
]

dir_config('symengine', HEADER_DIRS, LIB_DIRS)

# To avoid the error "can not be used when making a shared object; recompile with -fPIC"
$CFLAGS << ' -Wall' unless $CFLAGS.split.include?('-Wall')
$CFLAGS << ' -Werror' unless $CFLAGS.split.include?('-Werror')
$CFLAGS << ' -fPIC' unless $CFLAGS.split.include?('-fpic')
# $CFLAGS << ' -std=c99' unless $CFLAGS.split.include?('-std=c99')

RbConfig::CONFIG['CC'] = 'g++'
RbConfig::CONFIG['CXX'] = 'g++'
$CPPFLAGS='-Wall -O3 -Wall -Wcast-qual -Wconversion -DSGI__gnu_cxx -DREADLINE'
# Gives the ability to easily use alternate compilers to build the extension
#RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

# Giving it a name
extension_name = 'symengine/symengine'

# The destination
dir_config(extension_name)

# Check to see if the symengine library required to build this extension exists.
# Typically, we would want to use libsymengine installed, including the header
# files
# unless pkg_config('symengine')
#    raise "libsymengine not found"
# end
#unless have_header('cwrapper.h')
#    raise 'cwrapper.h not found'
#end
#unless have_library('symengine',
#                    'basic p; basic_init(p); basic_free(p)',
#                    'cwrapper.h')
#    abort 'Unable to find basic_init in library symengine!'
#end
$libs = append_library($libs, 'symengine')

# creates the header file extconf.h, based on the results from all of the
# previous have_*() functions.
# The extconf.h file will be included by all of the C files in the project
# to gain access to the HAVE_* macros that extconf defines.
create_header

create_makefile(extension_name)
