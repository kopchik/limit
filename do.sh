#!/bin/bash

make
killall -q experiment

sleep 0.1
taskset -ac 0 ./experiment > ./results/single
sleep 0.1
taskset -ac 1 ./experiment polute &
sleep 0.1
taskset -ac 0 ./experiment > ./results/double
sleep 0.1
taskset -ac 0 ./experiment `pidof experiment` > ./results/double_with_stop

killall experiment
