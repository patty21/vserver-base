#!/bin/sh


#file created by me to compile fastd
# fastd + libuecc
# https://projects.universe-factory.net/projects/fastd/wiki/Building
# https://projects.universe-factory.net/projects/fastd/files
#
# git: fastd
# http://git.universe-factory.net/fastd/
# git: libuecc
# http://git.universe-factory.net/libuecc/
#
# nacl: crypt lib wird dazugelinkt (keine shared lib)
# apt-get install libnacl-dev

#git
fastd_rev=281fbb005702da662e1658cb1bb4135f66a447bf
libuecc_rev=bb4fcb93282ca2c3440294683c88e8d54a1278e0

build_libuecc()
{
		
	# make sub shell to avoid extra calles to cd ..
	(
		rm -rf libuecc
		if [ -f libuecc-$libuecc_rev ]; then
			tar xzf libuecc-$libuecc_rev
		else
			git clone git://git.universe-factory.net/libuecc
			git checkout $libuecc_rev
			rev=$(git -C libuecc log -1 | sed -n '/^commit/s#commit ##p')
			tar czf libuecc-$rev.tgz libuecc
		fi
		cd libuecc
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

		rm -rf fastd
		if [ -f fastd-$fastd_rev ]; then
			tar xzf fastd-$fastd_rev
		else
			git clone git://git.universe-factory.net/fastd
			git checout $fastd_rev
			rev=$(git -C fastd log -1 | sed -n '/^commit/s#commit ##p')
			tar czf fastd-$rev.tgz fastd
		fi
	
		patch --directory=fastd -p0 < urandom.patch
	
		cd fastd
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

