#!/usr/bin/env python3
import pylab as p
from sys import argv

def f2list(fname):
  with open(fname, "rt") as fd:
    return [int(s) for s in fd]

#prefix = "results_limit_gran3_lat24_bws5"
prefix = "results"
prefix = argv[1]

single = f2list(prefix+"/single")
p.plot(single, label="single")
double = f2list(prefix+"/double")
p.plot(double, label="double")
with_stop = f2list(prefix+"/double_with_stop")
p.plot(with_stop, label="with stop")
p.legend(loc="best")
p.show()
