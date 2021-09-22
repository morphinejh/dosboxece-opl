#!/bin/bash
make distclean
find . -name "Makefile.in" -type f -delete
find ./src -type d -empty -delete

