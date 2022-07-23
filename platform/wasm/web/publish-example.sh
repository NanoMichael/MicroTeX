#!/usr/bin/env sh

if [ ! -d "example-dist" ]; then
  echo "Error: no example was created, exit." && exit 1
fi

cd "example-dist" || return

if [ ! -d ".git" ]; then
  echo "Init git..."
  git init
fi

git remote add origin git@github.com:NanoMichael/MicroTeX
git add .
git commit -m "publish page"
git push -f origin HEAD:microtex-page
