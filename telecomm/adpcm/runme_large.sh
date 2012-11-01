#!/bin/sh
bin/rawcaudio < data/large.pcm > output_large.adpcm
bin/rawdaudio < data/large.adpcm > output_large.pcm
