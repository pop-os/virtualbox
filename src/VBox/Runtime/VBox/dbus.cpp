/** @file
 *
 * Module to dynamically load libdbus and load all symbols
 * which are needed by VirtualBox.
 */

/*
 * Copyright (C) 2008 Sun Microsystems, Inc.
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 *
 * The contents of this file may alternatively be used under the terms
 * of the Common Development and Distribution License Version 1.0
 * (CDDL) only, as it comes in the "COPYING.CDDL" file of the
 * VirtualBox OSE distribution, in which case the provisions of the
 * CDDL are applicable instead of those of the GPL.
 *
 * You may elect to license modified versions of this file under the
 * terms and conditions of either the GPL or the CDDL or both.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 USA or visit http://www.sun.com if you need
 * additional information or have any questions.
 */

#define LOG_GROUP LOG_GROUP_MAIN
#include <VBox/log.h>
#include <VBox/dbus.h>
#include <VBox/err.h>

#include <iprt/ldr.h>
#include <iprt/assert.h>
#include <iprt/once.h>

/** The following are the symbols which we need from libdbus. */
#define VBOX_PROXY_STUB(function, rettype, signature, shortsig) \
void (*function ## _fn)(void); \
RTR3DECL(rettype) function signature \
{ return ( (rettype (*) signature) function ## _fn ) shortsig; }

#include <VBox/dbus-calls.h>

#undef VBOX_PROXY_STUB

/* Now comes a table of functions to be loaded from libdbus-1 */
typedef struct
{
    const char *name;
    void (**fn)(void);
} SHARED_FUNC;

#define VBOX_PROXY_STUB(s, dummy1, dummy2, dummy3 ) { #s , & s ## _fn } ,
static SHARED_FUNC SharedFuncs[] =
{
#include <VBox/dbus-calls.h>
    { NULL, NULL }
};
#undef VBOX_PROXY_STUB

/* The function which does the actual work for RTDBusLoadLib, serialised for
 * thread safety. */
DECLINLINE(int) loadDBusLibOnce(void *, void *)
{
    int rc = VINF_SUCCESS;
    RTLDRMOD hLib;

    LogFlowFunc(("\n"));
    rc = RTLdrLoad(VBOX_DBUS_1_3_LIB, &hLib);
    if (RT_FAILURE(rc))
        LogRelFunc(("Failed to load library %s\n", VBOX_DBUS_1_3_LIB));
    for (unsigned i = 0; RT_SUCCESS(rc) && SharedFuncs[i].name != NULL; ++i)
        rc = RTLdrGetSymbol(hLib, SharedFuncs[i].name, (void**)SharedFuncs[i].fn);
    LogFlowFunc(("rc = %Rrc\n", rc));
    return rc;
}

RTR3DECL(int) RTDBusLoadLib(void)
{
    static RTONCE sOnce = RTONCE_INITIALIZER;

    LogFlowFunc(("\n"));
    int rc = RTOnce (&sOnce, loadDBusLibOnce, NULL, NULL);
    LogFlowFunc(("rc = %Rrc\n", rc));
    return rc;
}