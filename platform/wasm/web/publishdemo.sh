#!/usr/bin/env sh

if [ ! -d "demo-dist" ]; then
  echo "Error: no demo was created, exit." && exit 1
fi

cd "demo-dist" || return

if [ ! -d ".git" ]; then
  echo "Init git..."
  git init
fi

git remote add origin git@github.com:NanoMichael/cLaTeXMath
git add .
git commit -m "publish page"
git push -f origin HEAD:microtex-page
