bin_PROGRAMS = kafkametareader messagesend

kafkametareader_SOURCES = meta_example.cpp

messagesend_SOURCES = send_message.cpp

INCLUDES = \
		-I $(top_srcdir)/src/common/ \
		-I $(top_srcdir)/src/messages/ \
		-I $(top_srcdir)/src/client/

LDADD = \
	$(top_builddir)/src/client/libkafkaclient.la
