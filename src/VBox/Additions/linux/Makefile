#
# Makefile for the VirtualBox Linux Guest Drivers.
#

#
#
# Copyright (C) 2009 Sun Microsystems, Inc.
#
# This file is part of VirtualBox Open Source Edition (OSE), as
# available from http://www.virtualbox.org. This file is free software;
# you can redistribute it and/or modify it under the terms of the GNU
# General Public License (GPL) as published by the Free Software
# Foundation, in version 2 as it comes in the "COPYING" file of the
# VirtualBox OSE distribution. VirtualBox OSE is distributed in the
# hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
#
# Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
# Clara, CA 95054 USA or visit http://www.sun.com if you need
# additional information or have any questions.
#

KBUILD_VERBOSE =

all:
	@echo "*** Building 'vboxadd' module ***"
	@$(MAKE) KBUILD_VERBOSE=$(KBUILD_VERBOSE) -C vboxadd
	@if [ -f vboxadd/vboxadd.ko ]; then \
	    cp vboxadd/vboxadd.ko .; \
	 else \
	    cp vboxadd/vboxadd.o .; \
	 fi
	@echo
	@if [ -d vboxvfs ]; then \
	    if [ -f vboxadd/Module.symvers ]; then \
	        cp vboxadd/Module.symvers vboxvfs; \
	    fi; \
	    echo "*** Building 'vboxvfs' module ***"; \
	    $(MAKE) KBUILD_VERBOSE=$(KBUILD_VERBOSE) -C vboxvfs; \
	    if [ -f vboxvfs/vboxvfs.ko ]; then \
	        cp vboxvfs/vboxvfs.ko .; \
	    else \
	        cp vboxvfs/vboxvfs.o .; \
	    fi; \
	fi


install:
	@$(MAKE) KBUILD_VERBOSE=$(KBUILD_VERBOSE) -C vboxadd install
	@if [ -d vboxvfs ]; then \
	    $(MAKE) KBUILD_VERBOSE=$(KBUILD_VERBOSE) -C vboxvfs install; \
	fi

clean:
	@$(MAKE) -C vboxadd clean
	@if [ -d vboxvfs ]; then \
	    $(MAKE) -C vboxvfs clean; \
	fi
	rm -f vboxadd.*o vboxvfs.*o

load:
	@/sbin/rmmod vboxvfs || true
	@/sbin/rmmod vboxadd || true
	@/sbin/insmod vboxadd.ko
	@if [ -f vboxvfs.ko ]; then /sbin/insmod vboxvfs.ko; fi
