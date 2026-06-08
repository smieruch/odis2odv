#!/bin/bash

if [ -z "$1" ]
then
    echo "usage: xproc.bash commit"
    echo 'example: xproc.bash "update readme"'
    exit
fi



commit="$1"

git add .    
git commit -m "$commit"
git push
