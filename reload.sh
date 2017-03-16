#!/bin/sh -e

if [ $# -lt 2 ]; then
	echo $0 \<kext_file\> \<kext_name\>
	exit 1
fi

FILE=$1
NAME=$2

sudo kextunload -b ${NAME} || echo $NAME not loaded
sudo rm -rf ${NAME}.kext
cp -R ${FILE} ./${NAME}.kext
sudo chown -R root:wheel ${NAME}.kext
sudo kextutil --verbose ${NAME}.kext