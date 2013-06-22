bin_PROGRAMS = kafkametareader

kafkametareader_SOURCES = meta_example.cpp

INCLUDES = \
		-I $(top_srcdir)/src/common/ \
		-I $(top_srcdir)/src/messages/ \
		-I $(top_srcdir)/src/client/

LDADD = \
	$(top_builddir)/src/client/libkafkaclient.la
