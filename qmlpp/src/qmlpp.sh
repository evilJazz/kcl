#!/bin/bash

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
