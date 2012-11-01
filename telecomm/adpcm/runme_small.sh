#!/bin/sh
bin/rawcaudio < data/small.pcm > output_small.adpcm
bin/rawdaudio < data/small.adpcm > output_small.pcm
