#!/usr/bin/env python3

from sys import argv

if __name__ == '__main__':
    if len(argv) != 3:
        raise OSError("Invalid number of script arguments!")
    
    f1 = open(argv[1], "r")
    f2 = open(argv[2], "r")
    
    cycle1 = [int(i) for i in f1.readline().split(' ')]
    cycle2 = [int(i) for i in f2.readline().split(' ')]

    # print(cycle1)
    # print(cycle2)

    k1 = len(cycle1)
    k2 = len(cycle2)

    assert k1 == k2, "Cycles of different length!"

    while k1 > 0 and cycle1[0] != cycle2[0]:
        cycle1 = cycle1[1:] + cycle1[:1]
        k1 -= 1
    
    assert k1 != 0, "No matching first node"

    for n1, n2 in zip(cycle1, cycle2):
        assert n1 == n2, f"Cycles don't match! {n1} != {n2}"
