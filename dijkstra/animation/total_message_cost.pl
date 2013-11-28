#! /usr/bin/perl -w
#
# Given output as specified for Assignment 4, compute the total cost
# by summing up the costs of each leg.  Record the start (first entry
# in column 1) and destination (last entry in column 2).
#
# Copyright 2013 Systems Deployment, LLC
# Author: Morris Bernstein (morris@systems-deployment.com)

# A moderately-complex regular expression can be made more readable
# by concatenating and commenting the individual clauses.
$pattern =
    '^' .			# start-of-line
    '(\\w+)' .			# word: first column (start location)
    '\W+' .			# non-word (whitespace & punctuation)
    '(\\w+)' .			# word: second column (end location)
    '\\W+' .			# non-word (whitespace & pujnctuation)
    '(\\d+)' .			# digits (leg cost)
    '$';			# end-of-line

# Read the first line to record the start location.
$_ = <>;
defined($_) or die "missing input?";

chomp;
/$pattern/x or die "bad input line 1";
($start, $end, $cost) = ($1, $2, $3);

# Read remaining lines tallying up the cost.
for (<>) {
    chomp;
    /$pattern/x or die "bad input line $.";
    $end = $2;
    $cost += $3;
}

print "$start\t$end\t$cost\n";
