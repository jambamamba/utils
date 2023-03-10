#!/bin/bash -xe
set -xe

module_name="$1"

if [ "${module_name}" == "" ]; then
   echo "need module_name as argument"
   exit -1
fi
path="$(cat .gitmodules | grep ${module_name}| grep path | sed -r 's/.*path = (.*)/\1/')"
git rm "${path}" --cached
rm -fr ".git/modules/${path}"
git config --remove-section submodule.${path}
rm -f /tmp/.gitmodules
cat .gitmodules | while read line; do hasmodule=$(echo $line | grep ${module_name}); if [ "$hasmodule" == "" ]; then echo "$line" >> /tmp/.gitmodules; fi; done
mv -f /tmp/.gitmodules .gitmodules
git add .gitmodules
rm -f tmp/.gitconfig
cat .git/config | while read line; do hasmodule=$(echo $line | grep ${module_name}); if [ "$hasmodule" == "" ]; then echo "$line" >> /tmp/.gitconfig; fi; done
mv /tmp/.gitconfig .git/config

