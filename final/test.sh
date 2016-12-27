#!/bin/bash

# ./test.sh 1 15 25
# ./test.sh 2 10 10
# ./test.sh 3 50 50

./first ./train/ruler/$1.jpg ./images/$1.jpg ./rulers/$1.jpg
# ./second ./rulers/$1.jpg ./train/e/neg/$1.jpg $2 $3 ./reade/$1.jpg
./second ./rulers/$1.jpg ./train/e/pos/$1.jpg $2 $3 ./reade/$1_pos.jpg
./second ./rulers/$1.jpg ./train/e/neg/$1.jpg $2 $3 ./reade/$1_neg.jpg
