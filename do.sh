#!/bin/bash

PREFIX=$1
  if [ -z "$PREFIX" ]; then
  echo "please specify prefix where to save data" 1>&2
  exit 1
fi
echo "saving results to $PREFIX" 1>&2
mkdir -p $PREFIX

make LPROF=1
killall -q experiment

sleep 0.1
taskset -ac 0 ./experiment > $PREFIX/single
sleep 0.1
taskset -ac 1 ./experiment polute &
sleep 0.1
taskset -ac 0 ./experiment > $PREFIX/double
sleep 0.1
taskset -ac 0 ./experiment `pidof experiment` > $PREFIX/double_with_stop

killall experiment
