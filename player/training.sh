# !/bin/bash
#
# training.sh -- training file for player
#
# usage: testing.sh 
# run this after running training.sh in dealer directory in separate terminal
#
# input: 
# output: 
#
# Alex Fick, CS50, Fall 2022

make train

for i in {1..2}
do
    ./train Name 129.170.65.170 8092
done

