# Copyright 2013 Systems Deployment, LLC
# Author: Morris Bernstein (morris@systems-deployment.com)

all: find_itinerary

test: reader_test priority_test


find_itinerary: graph.h flight_info.h ../time/time.h priority.h find_itinerary.cc flight_info.cc ../time/time.cc
	g++ -Wall -g -o find_itinerary find_itinerary.cc flight_info.cc ../time/time.cc

reader_test: graph.h flight_info.h ../time/time.h priority.h reader_test.cc flight_info.cc ../time/time.cc
	g++ -Wall -g -o reader_test reader_test.cc flight_info.cc ../time/time.cc

priority_test: priority.h priority_test.cc
	g++ -Wall -g -o priority_test priority_test.cc

priority_test_recursive: priority.h  priority_test.cc
	g++ -DRECURSIVE_IMPL -Wall -g -o priority_test_recursive priority_test.cc


clean:
	rm -f find_itinerary
	rm -f reader_test
	rm -f priority_test priority_test_recursive
	rm -f *.o
