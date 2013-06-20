lib_LTLIBRARIES = libkafkaclient.la

libkafkaclient_la_SOURCES = \
				client.cpp
            

include_HEADERS = \
				client.h
					

libkafkaclient_la_CPPFLAGS = -DBOOST_SPIRIT_THREADSAFE

libkafkaclient_la_CXXFLAGS = -Wall -Wextra -Weffc++

libkafkaclient_la_LIBADD = \
                    $(top_builddir)/src/common/libcommon.la \
                    -lboost_system -lzmq -lgrok -lboost_thread

INCLUDES = -I . -I$(srcdir)/src/common/

MAINTAINERCLEANFILES=Makefile.in
