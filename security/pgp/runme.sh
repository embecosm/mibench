#!/bin/sh
cd tests
../src/pgp -sa -z "this is a test" -u taustin@eecs.umich.edu testin.txt austin@umich.edu
../src/pgp -z "this is a test" -u taustin@eecs.umich.edu testout.txt.asc
