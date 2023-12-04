#!/usr/bin/env python3

from sys import argv
from random import randint, random
import numpy as np
import struct

def gen_w():
    return round(random(), 6)

if __name__ == '__main__':
    if len(argv) != 4:
        raise OSError("Invalid number of script arguments!")
    
    nodes = int(argv[1])
    edges = int(argv[2])
    calc_mcm = (argv[3] == "c")
    assert edges <= nodes**2
    uw_graph = {i+1:[(i+1) % nodes+1] for i in range(nodes)}

    for _ in range(edges - nodes):
        source = randint(1, nodes)
        target = source
        while target == source or target in uw_graph[source]:
            target = randint(1, nodes)
        uw_graph[source].append(target)

    graph = {v:[(i, gen_w()) for i in elist] for v, elist in uw_graph.items()}

    # V 7
    # E 8
    # 1 7 -2.000000
    # 7 6 3.000000
    # 6 5 -1.000000
    # 5 2 -1.000000
    # 2 1 4.000000
    # 2 3 2.000000
    # 3 4 1.000000
    # 4 5 3.000000
    # graph = {1:[(7, -2.000000)], 2:[(1, 4.000000), (3, 2.000000)], 3:[(4, 1.000000)], 4:[(5, 3.000000)], 5:[(2, -1.000000)], 6:[(5, -1.000000)], 7:[(6, 3.000000)]}

    with open(f"tests/{nodes}_{edges}.gr", "w") as fp_graph:
        fp_graph.write(f"V {nodes}\n") 
        fp_graph.write(f"E {edges}\n")
        for s, elist in graph.items():
            for t, w in elist:
                fp_graph.write(f"{t} {s} {w:.6f}\n")

    if calc_mcm:
        distances = np.matrix(np.ones((nodes, nodes)) * np.inf, dtype=np.longdouble)
        predecesors = np.matrix(np.ones((nodes, nodes)) * -1, dtype=np.int)

        distances[0, 0] = 0

        for k in range(nodes - 1):
            for s in graph.keys():
                if distances[k, s-1] is not np.inf:
                    for t, w in graph[s]:
                        if distances[k, s-1] + w < distances[k+1, t-1]:
                            distances[k+1, t-1] = distances[k, s-1] + w
                            predecesors[k+1, t-1] = s

        lks = np.ones((nodes)) * np.inf
        for v in range(nodes):
            if distances[-1, v] is not np.inf:
                lks[v] = -np.inf
                for k in range (nodes - 1):
                    lk = (distances[-1, v] - distances[k, v]) / (nodes - 1 - k)
                    if lk > lks[v]:
                        lks[v] = lk

        for v in range(nodes):
            if not np.isfinite(lks[v]):
                lks[v] = np.inf

        mcm = float("inf")
        end_ix = -1
        for ix, lk in enumerate(lks):
            if lk < mcm:
                mcm = lk
                end_ix = ix + 1

        look_ix = end_ix
        k = nodes - 1
        seen = [0 for _ in range(nodes)]
        while seen[look_ix - 1] == 0:
            seen[look_ix - 1] = k
            look_ix = predecesors[k, look_ix - 1]
            k -= 1
        cycle_ix = look_ix
        cycle_k = seen[look_ix - 1]

        cycle = []
        while cycle_ix not in cycle:
            cycle.append(cycle_ix)
            cycle_ix = predecesors[cycle_k, cycle_ix - 1]
            cycle_k -= 1

        cycle = list(reversed(cycle))

        mcm = 0.0
        for ix, s in enumerate(cycle):
            next = cycle[(ix + 1) % len(cycle)]
            for t, w in graph[s]:
                if next == t:
                    mcm += w
        mcm /= len(cycle)
                
        with open(f"tests/{nodes}_{edges}.mcm", "wb") as fp_mcm:
            b = struct.pack('f', mcm)
            fp_mcm.write(b)

                    
        with open(f"tests/{nodes}_{edges}.cycle", "w") as fp_cycle:
            fp_cycle.write(" ".join([str(i) for i in cycle])) 
            fp_cycle.write(f"\n") 

