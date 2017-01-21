#!/bin/bash

OUTPUT_PATH=xcode
if [ ! -d $OUTPUT_PATH ]
then
	echo "mkdir ${OUTPUT_PATH}"
	mkdir $OUTPUT_PATH
fi

QMAKE=$QT_COMPILER/clang_64/bin/qmake
echo "qmake path: ${QMAKE}"

$QMAKE CONFIG+=debug -spec macx-xcode -o ${OUTPUT_PATH}/qtcceditor qtcceditor.pro
#$QMAKE CONFIG+=debug -spec macx-xcode -o ${OUTPUT_PATH}/qtcceditor_v2 qtcceditor_v2.pro
