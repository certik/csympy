Steps to compile the extensions
===============================
* Build symengine as a shared library by adding the flag `-DBUILD_SHARED_LIBS=on`
* Install bundler by `gem install bundler`
* Execute `bundle install` this will install the gems required
* Then execute `rake compile` from the dir that contains Rakefile(`src/ruby`)
* Then execute `rake install` to install the gem.
