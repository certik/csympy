require 'mkmf'

#To avoid the error "can not be used when making a shared object; recompile with -fPIC"
$CFLAGS << ' -Wall' unless $CFLAGS.split.include?('-Wall')
$CFLAGS << ' -Werror' unless $CFLAGS.split.include?('-Werror')
$CFLAGS << ' -fPIC' unless $CFLAGS.split.include?('-fpic')

#Gives the ability to easily use alternate compilers to build the extension
RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

extension_name = 'symengine/symengine'

#Check to see if the symengine library required to build this extension exists.
#Typically, we would want to use libsymengine installed, including the header
#files
#unless pkg_config('symengine')
#    raise "libsymengine not found"
#end
have_library('symengine')

#creates the header file extconf.h, based on the results from all of the
#previous have_*() functions.
#The extconf.h file will be included by all of the C files in the project
#to gain access to the HAVE_* macros that extconf defines.
create_header

create_makefile(extension_name)
