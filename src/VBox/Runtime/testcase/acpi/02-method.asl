/* $Id: 02-method.asl $ */
/** @file
 * Runtime ACPI testcase - Simple method declaration.
 */

/*
 * Copyright (C) 2025 Oracle and/or its affiliates.
 *
 * This file is part of VirtualBox base platform packages, as
 * available from https://www.virtualbox.org.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, in version 3 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses>.
 *
 * SPDX-License-Identifier: GPL-3.0-only
 */
DefinitionBlock ("", "SSDT", 1, "VBOX  ", "VBOXTEST", 2)
{
    Method(TEST, 1, NotSerialized, 0)
    {
        If (LEqual(Arg0, One))
        {
            Return (One)
        } Else {
            Return (Zero)
        }
    }
}

