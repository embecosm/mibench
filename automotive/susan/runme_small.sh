#!/bin/sh
susan input_small.pgm output_small.smoothing.pgm -s
susan input_small.pgm output_small.edges.pgm -e
susan input_small.pgm output_small.corners.pgm -c

