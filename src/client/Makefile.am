lib_LTLIBRARIES = libkafkaclient.la

libkafkaclient_la_SOURCES = \
                command.cpp \
				connectionpool.cpp \
                commandexecutor.cpp \
                metaresponse.cpp \
                metacommand.cpp \
				produceresponse.cpp \
                producecommand.cpp \
				client.cpp
            

include_HEADERS = \
                types.h \
                partitioninfo.h \
                commandtype.h \
                command.h \
				connectionpool.h \
                commandexecutor.h \
				kafkaerrorcodes.h \
                metaresponse.h \
                metacommand.h \
                produceresponse.h \
                producecommand.h \
				client.h
					

libkafkaclient_la_CPPFLAGS = -DBOOST_SPIRIT_THREADSAFE

libkafkaclient_la_CXXFLAGS = -Wall -Wextra -Weffc++

libkafkaclient_la_LIBADD = \
                    $(top_builddir)/src/common/libcommon.la \
                    $(top_builddir)/src/messages/libmessages.la \
                    -lboost_system -lzmq -lgrok -lboost_thread

INCLUDES = -I . \
		-I$(top_srcdir)/src/common/ \
		-I$(top_srcdir)/src/messages/

MAINTAINERCLEANFILES=Makefile.in
