#!/bin/sh
bf e input_small.asc output_small.enc 1234567890abcdeffedcba0987654321
bf d output_small.enc output_small.asc 1234567890abcdeffedcba0987654321
