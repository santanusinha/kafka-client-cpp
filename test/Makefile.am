TESTS = kafka_client_test

check_PROGRAMS = kafka_client_test

kafka_client_test_SOURCES = \
			buffer_test.cpp

kafka_client_test_LDADD = \
                    $(top_builddir)/src/client/libkafkaclient.la \
					-lboost_unit_test_framework

kafka_client_test_LDFLAGS = $(all_libraries)

kafka_client_test_CPPFLAGS=-DBOOST_TEST_DYN_LINK

INCLUDES = \
			-I $(top_srcdir)/src/common/\
			-I $(top_srcdir)/src/client
			
