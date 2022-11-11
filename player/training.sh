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

make player # make sure train flag is uncommented

for i in {1..10}
do
    ./player Alex 129.170.65.170 8092
done

