AC_INIT([kafka-client-cpp], [0.01], [santanu.sinha@gmail.com], [kafka-client-cpp])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([./src/client/client.h])
AC_CONFIG_AUX_DIR([./config])

AC_PROG_CC
AC_PROG_CXX(g++)
AC_PROG_LIBTOOL

AM_INIT_AUTOMAKE([1.9])
AC_CONFIG_HEADERS([config.h])

AC_ARG_ENABLE(debug,
        [  --enable-debug          enable debug build (default=no)], ,
        enable_debug=no
)

AC_CONFIG_FILES([
Makefile
src/Makefile
src/common/Makefile
src/client/Makefile
test/Makefile
])

AC_OUTPUT
