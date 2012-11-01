#!/bin/sh
susan input_large.pgm output_large.smoothing.pgm -s
susan input_large.pgm output_large.edges.pgm -e
susan input_large.pgm output_large.corners.pgm -c

