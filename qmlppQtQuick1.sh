#!/bin/sh
qmlpp/src/qmlpp.sh -i -d "@QtQuick1" -q 1.0 src/qml
echo "QML and JS files rewritten for QtQuick 1."
