TESTS = \
    buffer_test \
    hostdetails_test

check_PROGRAMS = \
    buffer_test \
    hostdetails_test

buffer_test_SOURCES = buffer_test.cpp

hostdetails_test_SOURCES = hostdetails_test.cpp

LDADD = \
            $(top_builddir)/src/client/libkafkaclient.la \
            -lboost_unit_test_framework

AM_LDFLAGS = $(all_libraries)

AM_CPPFLAGS=-DBOOST_TEST_DYN_LINK

INCLUDES = \
			-I $(top_srcdir)/src/common/\
			-I $(top_srcdir)/src/client
			
