#! /usr/bin/python

import collections
import re
import sys

__copyright__ = "Copyright 2013 Systems Deployment, LLC"
__author__ = "Morris Bernstein"
__email__ = "morris@systems-deployment.com"

counter = collections.defaultdict(int)

for line in sys.stdin:
    for word in re.split('[^A-Za-z]+', line.strip()):
        if word:
            counter[word] += 1

for word, count in sorted(counter.items()):
    print "%7d %s" % (count, word)
