#! /usr/bin/python

"""Verify the input is in binary-heap order.

Input is from standard error output produced by priority_test.  Lines
are in the form:
**** extracted word: <word>
  <count>: word
Count is ignored.

http://en.wikipedia.org/wiki/Binary_heap
"""

import sys

__copyright__ = "Copyright 2013 Systems Deployment, LLC"
__author__ = "Morris Bernstein"
__email__ = "morris@systems-deployment.com"


def is_marker(line):
    result = False
    result = result or line.startswith("**** inserting word:")
    result = result or line.startswith("**** extracted word:")
    return result


line = sys.stdin.readline()
if not is_marker(line):
    print "invalid input: missing first marker"
    sys.exit(1)
print line,

while True:
    words = []
    for (n, line) in enumerate(sys.stdin):
        if is_marker(line):
            # Hit the next marker: done with current heap.  Break out
            # of the for loop to begin processing the next heap.
            print line,
            break
        # Word is second column per the input spec.
        word = line.split()[1].strip()
        print n, word
        words.append(word)
        if n > 0 and words[n] < words[(n + 1) // 2 - 1]:
            print "heap fail: node", n, "is less than its parent"
            sys.exit(1)
    else:
        # Normal termination of the for loop because end-of-input was
        # hit.  Break out of the enclosing while.
        break

print "heap ok"
