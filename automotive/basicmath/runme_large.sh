#!/bin/sh
echo basicmath_large
size basicmath_large
time ${RUN} ./basicmath_large > output_large.txt
