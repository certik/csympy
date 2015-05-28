require 'mkmf'

#Gives the ability to easily use alternate compilers to build the extension
RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

extension_name = 'symengine/symengine'

#Check to see if the csympy library required to build this extension exists.
#Typically, we would want to use libcsympy installed, including the header
#files
unless have_library("symengine")
    raise "libsymengine not found"
end
#How is libcsympy installed? Not able to find libcsympy

#creates the header file extconf.h, based on the results from all of the
#previous have_*() functions.
#The extconf.h file will be included by all of the C files in the project
#to gain access to the HAVE_* macros that extconf defines.
create_header

create_makefile(extension_name)
