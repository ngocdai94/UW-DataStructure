#! /bin/bash
#
# Copyright 2013 Systems Deployment, LLC
# Author: Morris Bernstein (morris@systems-deployment.com)

cat $* |                     # Files to process
sed -e 's/[^A-Za-z]/\n/g' |  # Convert non-word characters to newlines
grep -v '^$' |               # Discard blank lines
sort |                       # Sort words
uniq -c                      # Count repetitions
