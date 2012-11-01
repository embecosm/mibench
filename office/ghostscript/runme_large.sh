#!/bin/sh
src/gs -sDEVICE=ppm -dNOPAUSE -q -sOutputFile=data/output_large.ppm -- data/large.ps
