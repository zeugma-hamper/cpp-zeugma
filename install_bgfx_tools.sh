#!/bin/bash

INSTALL_BIN_DIR=/usr/local/bin

if [ ! -z "$1" ]; then
    INSTALL_BIN_DIR=$1
fi

SHADC='.build/linux64_gcc/bin/shadercRelease'
SHADC_DEST='shaderc'
GEOMC='.build/linux64_gcc/bin/geometrycRelease'
GEOMC_DEST='geometryc'
GEOMV='.build/linux64_gcc/bin/geometryvRelease'
GEOMV_DEST='geometryv'
TEXTC='.build/linux64_gcc/bin/texturecRelease'
TEXTC_DEST='texturec'
TEXTV='.build/linux64_gcc/bin/texturevRelease'
TEXTV_DEST='texturev'

cd bgfx
make -j10 tools

echo 'creating/checking ' $INSTALL_BIN_DIR
mkdir -p $INSTALL_BIN_DIR
echo 'installing '$SHADC_DEST' to '$INSTALL_BIN_DIR
cp $SHADC $INSTALL_BIN_DIR/$SHADC_DEST
echo 'installing '$GEOMC_DEST' to '$INSTALL_BIN_DIR
cp $GEOMC $INSTALL_BIN_DIR/$GEOMC_DEST
echo 'installing '$GEOMV_DEST' to '$INSTALL_BIN_DIR
cp $GEOMV $INSTALL_BIN_DIR/$GEOMV_DEST
echo 'installing '$TEXTC_DEST' to '$INSTALL_BIN_DIR
cp $TEXTC $INSTALL_BIN_DIR/$TEXTC_DEST
echo 'installing '$TEXTV_DEST' to '$INSTALL_BIN_DIR
cp $TEXTV $INSTALL_BIN_DIR/$TEXTV_DEST
