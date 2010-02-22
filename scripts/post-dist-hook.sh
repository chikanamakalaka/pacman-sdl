#!/bin/sh

# $1=distribution name
# $2=distribution version
# $3=package name of distribution
# $4=destination distribution directory name
echo 'post-dist-hook.sh'

#cp $4/$3 ./staging/$3.orig.tar.gz
cp pileofblocks-1.0.0.tar.gz ./staging/pileofblocks-1.0.0.orig.tar.gz

