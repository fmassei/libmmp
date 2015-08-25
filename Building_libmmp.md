#summary How to build lilbmmp
#labels Featured,Phase-Deploy

# Introduction #
libmmp is build like any other automake package

# Linux #

## From the downloaded package ##
  * **configure** has some useful options, like
    * **--disable-threads** to disable threads' functions wrapper
    * **--disable-sockets** to disable sockets' functions wrapper
    * **--disable-dl** to disable the dynamic loader wrapper
    * **--enable-debug** to enable the debug library version
  * **make**
  * **make check** to run the tests
  * **make install** (you will probably need to be root)
Note: The user's CFLAGS will be added to the package compiler options, so choose them wisely!

## From trunk ##
  * autoreconf --install
  * follow the above rules :)

# Windows #

In the folder named "vs10" you will find a Visual Studio Express 2010 solution and project (the windows project files cannot be updated to the latest versions).
Said project has tree build configuration:
  * **debug** will build the debug version
  * **release** will build the release version
  * **unittest** will build an application (named unittest) that will test the library and print out a TAP formatted report.