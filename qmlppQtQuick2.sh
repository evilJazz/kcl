#!/bin/sh
qmlpp/src/qmlpp.sh -i -d "@QtQuick2" -q 2.0 src/qml
echo "QML and JS files rewritten for QtQuick 2."
