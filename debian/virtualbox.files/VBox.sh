#!/bin/sh
## @file
# Oracle VM VirtualBox startup script, Linux hosts.
#

# written by Patrick Winnertz <patrick.winnertz@skolelinux.org> and
# Michael Meskes <meskes@debian.org>
# and placed under GPLv2
#
# this is based on a script by
# Oracle VirtualBox
#
# Copyright (C) 2006-2015 Oracle Corporation
#
# This file is part of VirtualBox Open Source Edition (OSE), as
# available from http://www.virtualbox.org. This file is free software;
# you can redistribute it and/or modify it under the terms of the GNU
# General Public License (GPL) as published by the Free Software
# Foundation, in version 2 as it comes in the "COPYING" file of the
# VirtualBox OSE distribution. VirtualBox OSE is distributed in the
# hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
#

PATH="/usr/bin:/bin:/usr/sbin:/sbin"

# VirtualBox installation directory
INSTALL_DIR="/usr/lib/virtualbox"

# Note: This script must not fail if the module was not successfully installed
#       because the user might not want to run a VM but only change VM params!

if [ ! -c /dev/vboxdrv ]; then
    cat << EOF
WARNING: The character device /dev/vboxdrv does not exist.
	 Please install the virtualbox-dkms package and the appropriate
	 headers, most likely linux-headers-$(uname -r | cut -d- -f3).

	 You will not be able to start VMs until this problem is fixed.
EOF
fi

SERVER_PID=`ps -U \`whoami\` | grep VBoxSVC | awk '{ print $1 }'`
if [ -z "$SERVER_PID" ]; then
    # Server not running yet/anymore, cleanup socket path.
    # See IPC_GetDefaultSocketPath()!
    if [ -n "$LOGNAME" ]; then
        rm -rf /tmp/.vbox-$LOGNAME-ipc > /dev/null 2>&1
    else
        rm -rf /tmp/.vbox-$USER-ipc > /dev/null 2>&1
    fi
fi

APP=`basename $0`
case "$APP" in
    VirtualBox|virtualbox)
        exec "$INSTALL_DIR/VirtualBox" "$@"
        ;;
    VirtualBoxVM|virtualboxvm)
        exec "$INSTALL_DIR/VirtualBoxVM" "$@"
        ;;
    VBoxManage|vboxmanage)
        exec "$INSTALL_DIR/VBoxManage" "$@"
        ;;
    VBoxSDL|vboxsdl)
        exec "$INSTALL_DIR/VBoxSDL" "$@"
        ;;
    VBoxVRDP|VBoxHeadless|vboxheadless)
        exec "$INSTALL_DIR/VBoxHeadless" "$@"
        ;;
    VBoxAutostart|vboxautostart)
        exec "$INSTALL_DIR/VBoxAutostart" "$@"
        ;;
    VBoxBalloonCtrl|vboxballoonctrl)
        exec "$INSTALL_DIR/VBoxBalloonCtrl" "$@"
        ;;
    VBoxBugReport|vboxbugreport)
        exec "$INSTALL_DIR/VBoxBugReport" "$@"
        ;;
    VBoxDTrace|vboxdtrace)
        exec "$INSTALL_DIR/VBoxDTrace" "$@"
        ;;
    vboxwebsrv)
        exec "$INSTALL_DIR/vboxwebsrv" "$@"
        ;;
    *)
        echo "Unknown application - $APP"
        exit 1
        ;;
esac
exit 0
