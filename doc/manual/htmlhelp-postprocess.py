#!/usr/bin/python3
# -*- coding: utf-8 -*-
# $Id: htmlhelp-postprocess.py $

"""
A python script to create post process html file in a gived folder. It
extracts href links to css files and looks for them in the same folder
as the html. If this search fails it looks for parent folders recursively
and if css file is found href link is updated in place.
"""

__copyright__ = \
"""
Copyright (C) 2006-2025 Oracle and/or its affiliates.

This file is part of VirtualBox base platform packages, as
available from https://www.virtualbox.org.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, in version 3 of the
License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <https://www.gnu.org/licenses>.

SPDX-License-Identifier: GPL-3.0-only
"""

import getopt
import logging
import os.path
import glob
import sys
import re

def css_in_parent(css_name, current_dir):
    css_path = os.path.normpath(os.path.join(current_dir, css_name))
    if os.path.isfile(css_path):
        return css_path
    parent_dir = os.path.dirname(current_dir)
    if parent_dir == current_dir:
        return ""
    return css_in_parent(css_name, parent_dir)

def check_css_references(folder):
    html_files = glob.glob(os.path.join(folder, '**', '*.html'), recursive=True)
    html_files += glob.glob(os.path.join(folder, '**', '*.htm'), recursive=True)
    css_href_pattern = re.compile(r'href="([^"]+\.css)"', re.IGNORECASE)
    for file_name in html_files:
        html_folder = os.path.dirname(file_name)
        with open(file_name, encoding="iso-8859-1", errors="replace") as file:
            content = file.read()
        updated = False
        matches = css_href_pattern.findall(content)
        for href in matches:
            css_path = os.path.normpath(os.path.join(html_folder, href))
            if os.path.isfile(css_path):
                continue
            # look for the css file in parent folder(s)
            found_path = css_in_parent(os.path.basename(css_path), html_folder)
            if found_path != "":
                new_css_path = os.path.relpath(found_path, html_folder)
                new_href = f'href="{new_css_path}"'
                old_href = f'href="{href}"'
                content = content.replace(old_href, new_href)
                logging.info(f'{old_href} is updated to {new_href} in {file_name}.')
                updated = True
        if updated:
            with open(file_name, 'w', encoding="iso-8859-1", errors="replace") as f:
                f.write(content)

def usage(iExitCode):
    print('htmlhelp-postprocess.py -d <helphtmlfolder> -o <outputfilename>')
    return iExitCode

def main(argv):
    # Parse arguments.
    helphtmlfolder = ''
    try:
        opts, _ = getopt.getopt(argv[1:], "hd:")
    except getopt.GetoptError as err:
        logging.error(str(err))
        return usage(2)
    for opt, arg in opts:
        if opt == '-h':
            return usage(0)
        if opt == "-d":
            helphtmlfolder = arg
    # check supplied helphtml folder argument
    if not helphtmlfolder:
        logging.error('No helphtml folder is provided. Exiting')
        return usage(2)
    if not os.path.exists(helphtmlfolder):
        logging.error('folder "%s" does not exist. Exiting', helphtmlfolder)
        return usage(2)
    helphtmlfolder = os.path.normpath(helphtmlfolder)
    check_css_references(helphtmlfolder)
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
