# Copyright 2013 Systems Deployment, LLC
# Author: Morris Bernstein (morris@systems-deployment.com)

all: find_itinerary

test: graph_reader_test priority_test

find_itinerary: find_itinerary.cc graph_reader.cc graph.cc flight.cc ../time/time.cc
	g++ -Wall -g -o find_itinerary find_itinerary.cc graph_reader.cc graph.cc flight.cc ../time/time.cc


graph_reader_test:    graph_reader_test.cc graph_reader.cc graph.cc flight.cc ../time/time.cc
	g++ -Wall -g -o graph_reader_test graph_reader_test.cc graph_reader.cc graph.cc flight.cc ../time/time.cc

priority_test: priority_test.cc
	g++ -Wall -g -o priority_test priority_test.cc


clean:
	rm -f find_itinerary
	rm -f graph_reader_test priority_test
