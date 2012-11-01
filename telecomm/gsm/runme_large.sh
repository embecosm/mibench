#!/bin/sh
bin/toast -fps -c data/large.au > output_large.encode.gsm
bin/untoast -fps -c data/large.au.run.gsm > output_large.decode.run
