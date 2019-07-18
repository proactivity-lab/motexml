#!/bin/bash

RELEASE_DATE=$(date -u +'%Y%m%d')

# Get the directory of the build-deb.sh file, will put the package here
PAK_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Go to the lib directory
cd $PAK_DIR/../lib

# Set checkinstall options
OPTIONS+=" --type=debian"
OPTIONS+=" --default"     # answer yes to any questions
OPTIONS+=" --fstrans"     # simulate filesystem so root permissions not needed
OPTIONS+=" --install=no"  # just make, don't install the package
OPTIONS+=" --nodoc"       # as long as actual docs don't exist

OPTIONS+=" --pakdir=$PAK_DIR"

OPTIONS+=" --pkgname=libmlformat"
OPTIONS+=" --pkgversion=0.1.0"
OPTIONS+=" --pkgrelease=$RELEASE_DATE"
OPTIONS+=" --provides=libmlformat"
OPTIONS+=" --pkglicense=BSD"
OPTIONS+=" --maintainer=prolab"

#OPTIONS+=" --exclude /usr/lib/python3"  # running lsb_release in a fresh container will make a pyc file there otherwise
OPTIONS+=" --exclude $(realpath $PAK_DIR/..)"    # anything created during compilation

# Run checkinstall
echo $OPTIONS
checkinstall $OPTIONS
