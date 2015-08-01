#!/bin/sh


#file created by me to compile fastd
# fastd + libuecc
# https://projects.universe-factory.net/projects/fastd/wiki/Building
# https://projects.universe-factory.net/projects/fastd/files
# http://fastd.readthedocs.org/en/v16/index.html
#
# nacl: crypt lib wird dazugelinkt (keine shared lib)
# apt-get install libnacl-dev

build_libuecc()
{	
	# make sub shell to avoid extra calles to cd ..
	(
		rm -rf libuecc-4
		tar xzf libuecc-4.tar.gz
		cd libuecc-4
		mkdir build
		cd build
		cmake ..
		make
		make install
		# call ldconfig to update search path of this lib; else fastd will not find this lib
		ldconfig
	)
}

build_fastd()
{
	(
		CMAKE_OPTIONS=" \
		-DCMAKE_BUILD_TYPE:STRING=MINSIZEREL \
		-DWITH_METHOD_CIPHER_TEST:BOOL=FALSE \
		-DWITH_METHOD_COMPOSED_GMAC:BOOL=FALSE \
		-DWITH_METHOD_GENERIC_GMAC:BOOL=FALSE \
		-DWITH_METHOD_GENERIC_POLY1305:BOOL=FALSE \
		-DWITH_METHOD_NULL:BOOL=FALSE \
		-DWITH_METHOD_XSALSA20_POLY1305:BOOL=FALSE \
		-DWITH_CIPHER_AES128_CTR:BOOL=FALSE \
		-DWITH_CIPHER_NULL:BOOL=TRUE \
		-DWITH_CIPHER_SALSA20:BOOL=FALSE \
		-DWITH_CIPHER_SALSA2012:BOOL=TRUE \
		-DWITH_MAC_GHASH:BOOL=FALSE \
		-DWITH_CMDLINE_USER:BOOL=FALSE \
		-DWITH_CMDLINE_LOGGING:BOOL=FALSE \
		-DWITH_CMDLINE_OPERATION:BOOL=FALSE \
		-DWITH_CMDLINE_COMMANDS:BOOL=FALSE \
		-DWITH_VERIFY:BOOL=FALSE \
		-DWITH_CAPABILITIES:BOOL=FALSE \
		-DENABLE_SYSTEMD:BOOL=FALSE \
		-DENABLE_LIBSODIUM:BOOL=FALSE \
		-DENABLE_LTO:BOOL=TRUE"

		rm -rf fastd-16
		tar xzf fastd-16.tar.gz
	
		patch --directory=fastd-16 -p0 < urandom.patch
	
		cd fastd-16
		mkdir build
		cd build
		cmake $CMAKE_OPTIONS ..
		make
		strip src/fastd
		ls -l src/fastd
		make install
	)
}

#needed libs
apt-get install libnacl-dev
apt-get install libjson0-dev

build_libuecc
build_fastd

#check if libuee is found
echo "--------- finished ------------------------"
echo "### call fastd --help"
fastd --help

