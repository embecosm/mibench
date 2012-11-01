#!/bin/sh
jpeg-6a/cjpeg -dct int -progressive -opt -outfile output_small_encode.jpeg input_small.ppm
jpeg-6a/djpeg -dct int -ppm -outfile output_small_decode.ppm input_small.jpg
