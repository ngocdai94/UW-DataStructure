# Copyright 2013 Systems Deployment, LLC
# Author: Morris Bernstein (morris@systems-deployment.com)

MAKEDEFS = /home/morris/uw/css343/2013-q4/make.defs

include $(MAKEDEFS)
include $(MAKERULES)

PROGS = \
	huff \
	puff


ZIP_FILES = \
	Run \
	bitstream.h \
	bitstream.cc \
	coding_table.h \
	coding_table.cc \
	frequency_table.h \
	frequency_table.cc \
	frequency_table_test.cc \
	huff.cc \
	huffman_tree.h \
	huffman_tree.cc \
	huffman_tree_test.cc \
	puff.cc \
	common-sense.txt

ZIP_DATA = \
	common-sense.txt



all : .progs

.progs: $(TARGET_PROGS)

.www: $(WWW_SRC_TARGET_DIR)/huffpuff.zip

zip: $(ZIP)/huffpuff.zip


huff: bitstream.cc coding_table.cc frequency_table.cc huffman_tree.cc huff.cc
	g++ -Wall -Werror -g -o $@ $^

puff: bitstream.cc coding_table.cc frequency_table.cc huffman_tree.cc puff.cc
	g++ -Wall -Werror -g -o $@ $^


test: frequency_table_test huffman_tree_test


frequency_table_test: frequency_table.cc frequency_table_test.cc
	g++ -Wall -Werror -g -o $@ $^

huffman_tree_test: frequency_table.cc huffman_tree.cc coding_table.cc huffman_tree_test.cc
	g++ -Wall -Werror -g -o $@ $^


$(ZIP)/huffpuff.zip: $(ZIP_FILES)
	rm -rf $(ZIP)/huffpuff
	mkdir -p $(ZIP)/huffpuff
	mkdir -p $(ZIP)/huffpuff/test
	cp $(ZIP_FILES) $(ZIP)/huffpuff/
	cp $(ZIP_DATA) $(ZIP)/huffpuff/test
	cd $(ZIP) && zip huffpuff.zip huffpuff/* hufpuff/data/*

$(WWW_SRC_TARGET_DIR)/huffpuff.zip: $(ZIP)/huffpuff.zip
	cp  $(ZIP)/huffpuff.zip $(WWW_SRC_TARGET_DIR)/huffpuff.zip


clean:
	rm -f huffman
	rm -f huff puff
	rm -f *.o
	rm -f *~
	rm -f *.tgz
	rm -f  $(DATA)/*.{huff,recover,dot,gif,ps}
