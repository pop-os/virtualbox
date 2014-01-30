#!/bin/sh

set -ex

UPSTREAM_VERSION=$2
ORIG_TARBALL=$3

REAL_TARBALL=`readlink -f ${ORIG_TARBALL}`

WORKING_DIR=`dirname ${ORIG_TARBALL}`

ORIG_TARBALL_DFSG=`echo ${ORIG_TARBALL} | sed -e "s/\(${UPSTREAM_VERSION}\)\(\.orig\)/\1-dfsg\2/g"`
ORIG_TARBALL_DIR=`echo ${ORIG_TARBALL_DFSG} | sed -e "s/_\(${UPSTREAM_VERSION}\)/-\1/g" -e "s/\.tar\.bz2//g"`
ORIG_TARBALL_DIR_STRIP=`basename ${ORIG_TARBALL_DIR}`
DEST_TARBALL_DIR=${ORIG_TARBALL_DIR%.orig}
DEST_TARBALL_DIR_STRIP=`basename ${DEST_TARBALL_DIR}`

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


tar --exclude .svn --exclude '.git*' --exclude debian --directory ${WORKING_DIR} -cJf ${DEST_TARBALL_DIR}.tar.xz ${ORIG_TARBALL_DIR_STRIP} || exit 1
rm -rf ${ORIG_TARBALL_DIR}
echo "Done, now you can run git-import-orig --pristine-tar ${DEST_TARBALL_DIR}.tar.xz"
exit 0
