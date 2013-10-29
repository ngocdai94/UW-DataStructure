#! /usr/bin/python
#

"""Random permutation of groups of 5 lines.

Usage: permute5.py [seed]

This program generated nearly-sorted data for demostrating the
construction of a binary search tree or balanced (red-black) binary
search tree.
"""


# Given a text file, say a sorted list of words, randomly permute
# groups of 5 lines.

import random
import sys


__copyright__ = "Copyright 2013 Systems Deployment, LLC"
__author__ = "Morris Bernstein"
__email__ = "morris@systems-deployment.com"


if sys.argv[1:]:
    random.seed(int(sys.argv[1]))
else:
    random.seed()

lines = sys.stdin.readlines()
permuted = []
for i in range(0, len(lines), 5):
    subseq = lines[i:i+5]
    random.shuffle(subseq)
    permuted += subseq
[sys.stdout.write(line) for line in permuted]
