#!/bin/bash
make clean
find . -exec touch {} \;
make
