#! /usr/bin/env sh

REPO=../pysensors-git-pages

cd doc && make html && cd -
cp -r doc/build/html/* $REPO
cd $REPO
git add .
git commit -am "Update generated files"
git push origin gh-pages

