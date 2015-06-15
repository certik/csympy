# Steps to compile the extensions

Install openssl into `$HASHSTACK`, on my machine
$HASHSTACK=`/local/certik/bld/profile/xt33omorophg`.

## Install RVM

None of this requires root access.

Follow the instructions at https://rvm.io to install RVM, e.g.:

    gpg --keyserver hkp://keys.gnupg.net --recv-keys 409B6B1796C275462A1703113804BB82D39DC0E3
    curl -sSL https://get.rvm.io | bash -s stable

Then use RVM to install Ruby and dependencies:

    rvm autolibs disable
    rvm install ruby --with-openssl-dir=$HASHSTACK
    gem install bundler

## Install SymEngine

    cmake -DCOMMON_DIR=$HASHSTACK -DWITH_RUBY=yes -DBUILD_SHARED_LIBS=yes .
    make

## Install the Ruby wrappers

    cd src/ruby
    bundle install

Now apply the following patch:

    --- a/src/ruby/ext/symengine/extconf.rb
    +++ b/src/ruby/ext/symengine/extconf.rb
    @@ -14,6 +14,9 @@ HEADER_DIRS = [
         # Check the ruby install locations
         INCLUDEDIR,
     
    +    '/home/certik/repos/symengine/src',
    +    '/local/certik/bld/profile/xt33omorophg/include',
    +
         # Finally fall back to /usr
         '/usr/include'
     ]

and build the wrappers:

    rake compile
    rake install
