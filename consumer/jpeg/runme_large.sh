#!/bin/sh
jpeg-6a/cjpeg -dct int -progressive -opt -outfile output_large_encode.jpeg input_large.ppm
jpeg-6a/djpeg -dct int -ppm -outfile output_large_decode.ppm input_large.jpg
