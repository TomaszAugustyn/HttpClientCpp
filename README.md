# HttpClientCpp
Http client in C++ for monitoring devices in Home Center

## Requirements ##

### Boost ###

  * Download latest boost from https://www.boost.org/users/download/
  * $ sudo apt-get install build-essential python-dev autotools-dev libicu-dev libbz2-dev 
  * $ tar -xvzf boost_1_69_0.tar.gz
  * $ cd boost_1_69_0
  * $ sudo ./bootstrap.sh --prefix=/usr/local --with-libraries=all
  * $ sudo ./b2 install
  * Add the Boost libraries path to the default Ubuntu library search path <br />
	$ sudo /bin/bash -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/boost.conf'
  * Update the default Ubuntu library search paths <br />
	$ sudo ldconfig


### gtest ###

  * Get the googletest framework: https://github.com/google/googletest/archive/release-1.8.1.tar.gz
  * $ sudo apt-get install cmake
  * $ tar xf googletest-release-1.8.1.tar.gz
  * $ cd googletest-release-1.8.1/googletest
  * $ cmake -DBUILD_SHARED_LIBS=ON .
  * $ make
  * $ sudo cp -a include/gtest /usr/include
  * $ sudo cp -a libgtest_main.so libgtest.so /usr/lib/
  * Check if the GNU Linker knows the libs <br />
	$ sudo ldconfig -v | grep gtest
	

### libcurl ###

  * $ sudo apt-get install libcurl4-gnutls-dev
  * Check Linker <br />
	$ sudo ldconfig -v | grep curl 
  * Append LIBS with -lcurl in your Makefile


### JsonCPP ###

  * $ sudo apt-get install libjsoncpp-dev
  * Append LIBS with -ljsoncpp in your Makefile
  

## Building and running  ##
 
  * $ git clone https://github.com/TomaszAugustyn/HttpClientCpp.git
  * $ cd HttpClientCpp
  * Build application and tests from root directory <br />
    $ make
  * To run application: <br />
    $ cd src/ <br />
	$ make run
  * To run tests: <br />
    $ cd test/ <br />
	$ make runTests
	

 
 
 
 
