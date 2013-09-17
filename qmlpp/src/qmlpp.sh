#!/bin/bash
#
# qmlpp - KISS QML and JavaScript preprocessor
#
# Copyright (C) 2013 Andre Beckedorf
#       <evilJazz _AT_ katastrophos _DOT_ net>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version
# 2.1 as published by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301  USA
#
# Alternatively, this file is available under the Mozilla Public
# License Version 1.1.  You may obtain a copy of the License at
# http://www.mozilla.org/MPL/

function preprocessFile()
{
    sed -r \
        $additionalSedArgs -e "$rewriteParams" \
        -e "s/^(\s*)(\/\/)?(.*)\/\/@(.*)/\1\3\/\/@\4/" \
        -e "/($defines)/!s/^(\s*)(.*)\/\/(@.*)/\1\/\/\2\/\/\3/" \
        "$1"
}

function preprocessDirectory()
{
    find "$1" -type f | grep -v .svn | grep -v .git | while read file; do
            preprocessFile "$file"
    done
}

function usage()
{
    cat << EOF
usage: $0 [options] <filename JS or QML or directoryname>

OPTIONS:
   -h                Show this message.
   -q <major.minor>  The version of QtQuick to rewrite to. Example: -q 2.1
   -d <defines>      The defines to set, separated by |. Example: -d "@QtQuick1|@Meego|@Debug"
   -i                Modify file in-place instead of dumping to stdout.
EOF
}

processInline=false
additionalSedArgs=
defines=@NODEFINESET

while getopts ":hiq:d:" OPTION; do
    case $OPTION in
        q)
            rewriteQtQuickVersion="$OPTARG"
            ;;
        d)
            defines="$OPTARG"
            ;;
        i)
            processInline=true
            additionalSedArgs="-i"
            ;;
        h)
            usage
            exit 1
            ;;
        ?)
            echo "Invalid option: -$OPTARG" >&2
            usage
            exit 1
            ;;
    esac
done

rewriteParams=""
[ "$rewriteQtQuickVersion" != "" ] && rewriteParams="/\/\/!noRewrite/!s/(import QtQuick)\s*[0-9].[0-9]/\1 $rewriteQtQuickVersion/"

input="${@: -1}"

if [[ -f "$input" || "$input" == "-" ]]; then
        preprocessFile "$input"
elif [ -d "$input" ]; then
        if [ "$processInline" != "true" ]; then
            echo "Please specify -i when trying to preprocess a whole directory recursively."
            usage
            exit 1
        fi

        preprocessDirectory "$input"
else
        echo "Please specify a valid file or directory."
        usage
        exit 1
fi
