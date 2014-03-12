#!/bin/bash

PERF=/home/sources/abs/core/linux/src/linux-3.13/tools/perf/perf
OUTPUT="perf_stat"
WARMUP="sleep 0.1"
./experiment polute &
BGPID=$!
$WARMUP

./experiment polute &
FGPID=$!
sleep 0.1
$WARMUP

$PERF stat -I 1 -e cycles,instructions -x, -p $FGPID -o $OUTPUT sleep 0.1 &

kill -STOP $BGPID
sleep 0.3
killall -KILL experiment
