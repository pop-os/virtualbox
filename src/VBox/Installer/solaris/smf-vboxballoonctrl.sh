#!/sbin/sh
# $Id: smf-vboxballoonctrl.sh $

# Copyright (C) 2008-2013 Oracle Corporation
#
# This file is part of VirtualBox Open Source Edition (OSE), as
# available from http://www.virtualbox.org. This file is free software;
# you can redistribute it and/or modify it under the terms of the GNU
# General Public License (GPL) as published by the Free Software
# Foundation, in version 2 as it comes in the "COPYING" file of the
# VirtualBox OSE distribution. VirtualBox OSE is distributed in the
# hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
#

#
# smf-vboxballoonctrl method
#
# Argument is the method name (start, stop, ...)

. /lib/svc/share/smf_include.sh

VW_OPT="$1"
VW_EXIT=0

case $VW_OPT in
    start)
        if [ ! -x /opt/VirtualBox/VBoxBalloonCtrl ]; then
            echo "ERROR: /opt/VirtualBox/VBoxBalloonCtrl does not exist."
            return $SMF_EXIT_ERR_CONFIG
        fi

        if [ ! -f /opt/VirtualBox/VBoxBalloonCtrl ]; then
            echo "ERROR: /opt/VirtualBox/VBoxBalloonCtrl does not exist."
            return $SMF_EXIT_ERR_CONFIG
        fi

        # Get svc configuration
        VW_USER=`/usr/bin/svcprop -p config/user $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_USER=
        VW_INTERVAL=`/usr/bin/svcprop -p config/interval $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_INTERVAL=
        VW_INCREMENT=`/usr/bin/svcprop -p config/increment $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_INCREMENT=
        VW_DECREMENT=`/usr/bin/svcprop -p config/decrement $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_DECREMENT=
        VW_LOWERLIMIT=`/usr/bin/svcprop -p config/lowerlimit $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_LOWERLIMIT=
        VW_SAFETYMARGIN=`/usr/bin/svcprop -p config/safetymargin $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_SAFETYMARGIN=
        VW_ROTATE=`/usr/bin/svcprop -p config/logrotate $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_ROTATE=
        VW_LOGSIZE=`/usr/bin/svcprop -p config/logsize $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_LOGSIZE=
        VW_LOGINTERVAL=`/usr/bin/svcprop -p config/loginterval $SMF_FMRI 2>/dev/null`
        [ $? != 0 ] && VW_LOGINTERVAL=

        # Provide sensible defaults
        [ -z "$VW_USER" ] && VW_USER=root

        # Assemble the parameter list
        PARAMS="--background"
        [ -n "$VW_INTERVAL" ]     && PARAMS="$PARAMS --balloon-interval \"$VW_INTERVAL\""
        [ -n "$VW_INCREMENT" ]    && PARAMS="$PARAMS --balloon-inc \"$VW_INCREMENT\""
        [ -n "$VW_DECREMENT" ]    && PARAMS="$PARAMS --balloon-dec \"$VW_DECREMENT\""
        [ -n "$VW_LOWERLIMIT" ]   && PARAMS="$PARAMS --balloon-lower-limit \"$VW_LOWERLIMIT\""
        [ -n "$VW_SAFETYMARGIN" ] && PARAMS="$PARAMS --balloon-safety-margin \"$VW_SAFETYMARGIN\""
        [ -n "$VW_ROTATE" ]       && PARAMS="$PARAMS -R \"$VW_ROTATE\""
        [ -n "$VW_LOGSIZE" ]      && PARAMS="$PARAMS -S \"$VW_LOGSIZE\""
        [ -n "$VW_LOGINTERVAL" ]  && PARAMS="$PARAMS -I \"$VW_LOGINTERVAL\""

        exec su - "$VW_USER" -c "/opt/VirtualBox/VBoxBalloonCtrl $PARAMS"

        VW_EXIT=$?
        if [ $VW_EXIT != 0 ]; then
            echo "VBoxBalloonCtrl failed with $VW_EXIT."
            VW_EXIT=1
        fi
    ;;
    stop)
        # Kill service contract
        smf_kill_contract $2 TERM 1
    ;;
    *)
        VW_EXIT=$SMF_EXIT_ERR_CONFIG
    ;;
esac

exit $VW_EXIT
