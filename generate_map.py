#!/usr/bin/env python

import itertools as it

channels = range(68)
cells = range(512)
agets = range(4)

sides = range(2)
rings = range(31)
pads = range(128)

raw_ids = it.product(channels,cells,agets)
pad_ids = it.product(sides,rings,pads)

for raw,pad in it.izip(raw_ids,pad_ids):
    print raw[0]+1,raw[1]+1,raw[2]+1,pad[0],pad[1],pad[2]
