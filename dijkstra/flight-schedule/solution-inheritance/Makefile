# Copyright 2013 Systems Deployment, LLC
# Author: Morris Bernstein (morris@systems-deployment.com)

HEADERS= flight_reader.h flight.h ../time/time.h graph.h


all: find_itinerary

test: reader_test


find_itinerary: $(HEADERS) find_itinerary.cc flight_reader.cc flight.cc ../time/time.cc graph.cc
	g++ -Wall -g -o find_itinerary find_itinerary.cc flight_reader.cc flight.cc ../time/time.cc graph.cc


reader_test: $(HEADERS) reader_test.cc flight_reader.cc flight.cc ../time/time.cc graph.cc
	g++ -Wall -g -o reader_test reader_test.cc flight_reader.cc flight.cc ../time/time.cc graph.cc


clean:
	rm -f find_itinerary
	rm -f reader_test
	rm -f priority_test priority_test_recursive
	rm -f *.o
	rm -f *.out *.err
