#!/usr/bin/env python

import itertools as it

channels = range(68)
cells = range(512)
agets = range(4)

rings = range(32)

npads = [
     48,  48,  72,  96, 120, 144, 168, 192, 216, 240,
    208, 218, 230, 214, 212, 214, 220, 224, 232, 238,
    244, 232, 218, 210, 206, 202, 200, 196, 178, 130,
    108,  90 ]

pad_ids = []
for ring in rings:
    for pad in range(npads[ring]):
        pad_ids += [(ring,pad)]


raw_ids = it.product(agets,channels,cells)

for raw,pad in it.izip(raw_ids,pad_ids):
    print raw[0],raw[1],raw[2],pad[0],pad[1]
