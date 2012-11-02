#!/bin/sh

# A script to run one MiBench program remotely.

# Copyright 2012 Embecosm Limted

# Contributed by Jeremy Bennett <jeremy.bennett@embecosm.com>

# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.

# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.

# Single argument is the program to run.
progname=$1

# Print the name and size of the program
echo ${progname}
size ${progname}

# Copy the program. We use kermit to avoid FTP hassle (yum install ckermit).
echo "Sending file..." > /tmp/log
kermit -M root <<EOF >> /tmp/log
ftp open 192.168.0.110
user root
binary
put ${progname} ${progname}
bye
EOF

# Run the program
echo "Running commands..." >> /tmp/log
expect <<EOF >> /tmp/log
set timeout 300
spawn telnet 192.168.0.110
expect "# "
send "chmod ugo+x ${progname}\n"
expect "# "
send "time ./${progname} > /dev/null\n"
expect "# "
send "rm ./${progname}\n"
expect "# "
send "exit\n"
EOF

# /tmp/log should contain the timing information
egrep "^real    " < /tmp/log
egrep "^user    " < /tmp/log
egrep "^sys     " < /tmp/log
