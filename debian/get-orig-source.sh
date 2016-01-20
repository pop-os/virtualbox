#!/bin/sh

set -ex

UPSTREAM_VERSION=$2
ORIG_TARBALL=$3

REAL_TARBALL=`readlink -f ${ORIG_TARBALL}`

WORKING_DIR=`dirname ${ORIG_TARBALL}`

ORIG_TARBALL_DFSG=`echo ${ORIG_TARBALL} | sed -e "s/\(${UPSTREAM_VERSION}\)\(\.orig\)/\1-dfsg/g"`
ORIG_TARBALL_DIR=`echo ${ORIG_TARBALL_DFSG} | sed -e "s/_\(${UPSTREAM_VERSION}\)/-\1/g" -e "s/\.tar\.bz2//g"`
ORIG_TARBALL_DIR_STRIP=`basename ${ORIG_TARBALL_DIR}`
DEST_TARBALL_NAME=`echo ${ORIG_TARBALL_DIR} | sed -e "s#-\(${UPSTREAM_VERSION}\)#_\1#g"`.orig.tar.xz

mkdir -p ${ORIG_TARBALL_DIR}
tar --directory=${ORIG_TARBALL_DIR} --strip 1 -xjf ${REAL_TARBALL} || exit 1
rm -f  ${ORIG_TARBALL} ${REAL_TARBALL}

rm -fr ${ORIG_TARBALL_DIR}/debian
rm -fr ${ORIG_TARBALL_DIR}/kBuild
rm -fr ${ORIG_TARBALL_DIR}/tools
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Additions/os2
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Additions/WINNT
rm -f  ${ORIG_TARBALL_DIR}/VBox/HostDrivers/Support/darwin/load.sh
rm -f  ${ORIG_TARBALL_DIR}/include/VBox/VBoxGuest.inc
rm -f  ${ORIG_TARBALL_DIR}/include/VBox/VBoxGuest16.h
rm -f  ${ORIG_TARBALL_DIR}/include/VBox/VBoxGuest.mac
rm -f  ${ORIG_TARBALL_DIR}/src/libs/xpcom18a4/xpcom/MoreFiles/FSCopyObject.c
rm -f  ${ORIG_TARBALL_DIR}/src/libs/xpcom18a4/xpcom/MoreFiles/FSCopyObject.h
rm -fr ${ORIG_TARBALL_DIR}/src/libs/libpng*
rm -fr ${ORIG_TARBALL_DIR}/src/libs/libxml2*
rm -fr ${ORIG_TARBALL_DIR}/src/libs/libxslt*
rm -fr ${ORIG_TARBALL_DIR}/src/libs/zlib*
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Additions/linux/selinux-fedora
find   ${ORIG_TARBALL_DIR}/src/VBox/Additions/x11/x11include -mindepth 1 -maxdepth 1 \
   -type d ! -name '*mesa-*' -exec rm -rf {} \;
rm -fr ${ORIG_TARBALL_DIR}/src/libs/kStuff/kStuff/kLdr/testcase/bin
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Devices/EFI/Firmware/FatBinPkg/EnhancedFatDxe/Ebc/Fat.efi
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Devices/EFI/Firmware/FatBinPkg/EnhancedFatDxe/Ia32/Fat.efi
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Devices/EFI/Firmware/FatBinPkg/EnhancedFatDxe/Ipf/Fat.efi
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Devices/EFI/Firmware/FatBinPkg/EnhancedFatDxe/X64/Fat.efi
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Devices/EFI/Firmware/StdLib/LibC/Main/Ia32/ftol2.obj
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Devices/EFI/Firmware/BaseTools/Source/Python/UPT/Dll/sqlite3.dll
rm -fr ${ORIG_TARBALL_DIR}/src/VBox/Main/webservice/jaxlibs/*.jar
rm -rf ${ORIG_TARBALL_DIR}/src/VBox/HostDrivers/Support/win/winstub.com

tar --exclude .svn --exclude '.git*' --exclude debian --directory ${WORKING_DIR} -cJf ${DEST_TARBALL_NAME} ${ORIG_TARBALL_DIR_STRIP} || exit 1
rm -rf ${ORIG_TARBALL_DIR}
echo "Done, now you can run gbp import-orig ${DEST_TARBALL_NAME}"
exit 0
