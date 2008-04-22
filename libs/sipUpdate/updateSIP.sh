#! /bin/sh
#
#  Copyright (C) 2007 Savoir-Faire Linux inc.
#  Author: Jean Tessier <jean.tessier@polymtl.ca>
#                                                                              
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#                                                                                
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#                                                                              
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# This script uninstalls old versions of libosip and libeXosip(Ubuntu 
# Gutsy repository version). Downloads the new versions and installs them.
#
# Warning: This script uninstalls all dependecies of libosip and libeXosip.

# Removing old Versions
sudo apt-get remove libexosip2-5 libexosip2-dev libosip2-3 libosip2-dev

# Exit on Error
set -o errexit

# Get new version of librairies
wget http://www.antisip.com/download/exosip2/libeXosip2-3.1.0.tar.gz
wget http://www.antisip.com/download/exosip2/libosip2-3.1.0.tar.gz

# Untar the libairies
tar -xvzf libosip2-3.1.0.tar.gz
tar -xvzf libeXosip2-3.1.0.tar.gz

# Compile and install libosip2
cd libosip2-3.1.0
mkdir bld-linux
cd bld-linux
../configure --prefix=/usr
make
sudo make install
sudo ldconfig -v
sudo ldconfig
cd ../../

# Compile and install libeXosip2
cd libeXosip2-3.1.0
mkdir bld-linux
cd bld-linux
../configure --prefix=/usr
make
sudo make install
sudo ldconfig -v
sudo ldconfig
cd ../../
