#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dataComp], [0.99], [Mario.Sitta@cern.ch])
AC_CONFIG_SRCDIR([hiclib.cpp])
#AC_CONFIG_HEADERS([config.h])
#AM_INIT_AUTOMAKE([-Wall -Werror foreign])
#AM_INIT_AUTOMAKE([foreign])
AM_INIT_AUTOMAKE()

# Checks for programs.
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11
AC_PROG_CC

# Custom variables.
AC_SUBST(libxml2inc)
AC_SUBST(rootdir)
AC_SUBST(alpidesw)
AC_SUBST(alpideswinc)
AC_SUBST(alpidedbinc)
AC_SUBST(alpidepbinc)
AC_SUBST(alpidelibmosinc)
AC_SUBST(alpidelibs)

# Custom flags.
AC_ARG_WITH([libxml2-inc],
    AC_HELP_STRING([--with-libxml2-inc=DIR], [location of libxml2 include [/usr/include/libxml2]]),
    [libxml2inc="$withval"])
AS_IF([test "x$libxml2inc" == "x"],
    AC_SUBST(libxml2inc,"/usr/include/libxml2"))
AC_SUBST(CFLAGS,"$CFLAGS -I$libxml2inc")
AC_SUBST(CXXFLAGS,"$CXXFLAGS -I$libxml2inc")

AC_ARG_WITH([root],
    AC_HELP_STRING([--with-root=DIR], [location of ROOT installation [/opt/root]]),
    [rootdir="$withval"])
AS_IF([test "x$rootdir" == "x"],
    AC_SUBST(rootdir,"/opt/root"))

AC_ARG_WITH([alpide-sw],
    AC_HELP_STRING([--with-alpide-sw=ALPDIR], [location of Alpide software tree]),
    [alpidesw="$withval"])

AC_ARG_WITH([alpide-sw-inc],
    AC_HELP_STRING([--with-alpide-sw-inc=DIR], [location of Alipde software include [ALPDIR/inc]]),
    [alpideswinc="$withval"])
AS_IF([test "x$alpideswinc" == "x"],
    AC_SUBST(alpideswinc,"$alpidesw/inc"))
AC_SUBST(CXXFLAGS,"$CXXFLAGS -I$alpideswinc")

AC_ARG_WITH([alpide-db-inc],
    AC_HELP_STRING([--with-alpide-db-inc=DIR], [location of Alipde DB software include [ALPDIR/DataBaseSrc]]),
    [alpidedbinc="$withval"])
AS_IF([test "x$alpidedbinc" == "x"],
    AC_SUBST(alpidedbinc,"$alpidesw/DataBaseSrc"))
AC_SUBST(CXXFLAGS,"$CXXFLAGS -I$alpidedbinc")

AC_ARG_WITH([alpide-pb-inc],
    AC_HELP_STRING([--with-alpide-pb-inc=DIR], [location of Alipde PowerBoard software include [ALPDIR/MosaicSrc/libpowerboard/include]]),
    [alpidepbinc="$withval"])
AS_IF([test "x$alpidepbinc" == "x"],
    AC_SUBST(alpidepbinc,"$alpidesw/MosaicSrc/libpowerboard/include"))
AC_SUBST(CXXFLAGS,"$CXXFLAGS -I$alpidepbinc")

AC_ARG_WITH([alpide-libmosaic-inc],
    AC_HELP_STRING([--with-alpide-libmosaic-inc=DIR], [location of Alipde Mosaic software include [ALPDIR/MosaicSrc/libmosaic/include]]),
    [alpidelibmosinc="$withval"])
AS_IF([test "x$alpidelibmosinc" == "x"],
    AC_SUBST(alpidelibmosinc,"$alpidesw/MosaicSrc/libmosaic/include"))
AC_SUBST(CXXFLAGS,"$CXXFLAGS -I$alpidelibmosinc")

AC_ARG_WITH([alpide-libs],
    AC_HELP_STRING([--with-alpide-libs=DIR], [location of Alipde software libraries [ALPDIR/lib]]),
    [alpidelibs="$withval"])
AS_IF([test "x$alpidelibs" == "x"],
    AC_SUBST(alpidelibs,"$alpidesw/lib"))
AC_SUBST(LIBS,"$LIBS -L$alpidelibs")

# Checks for prerequisites.
AC_CHECK_HEADER([libxml/parser.h],[],[AC_MSG_ERROR([Unable to find libxml/parser.h])],[#include <stdio.h>])

AC_CHECK_PROG([root_config_found],[root-config],[yes],[no],[$rootdir/bin])
AS_IF([test "x$root_config_found" != "xyes"],
    AC_MSG_ERROR([Unable to find root-config]))

AC_SUBST(ROOTCFLAGS,"$($rootdir/bin/root-config --cflags)")
AC_SUBST(CXXFLAGS,"$CXXFLAGS $ROOTCFLAGS")

AC_LANG(C++)
AC_CHECK_HEADER([Rtypes.h],[],[AC_MSG_ERROR([Unable to find ROOT include files])],[#include <stdio.h>])
AC_LANG(C)

AC_SUBST(ROOTLIBS,"$($rootdir/bin/root-config --libs)")
AC_SUBST(ROOTGLIBS,"$($rootdir/bin/root-config --glibs)")
AC_SUBST(LIBS,"$LIBS $ROOTGLIBS")

AC_LANG(C++)
AC_CHECK_LIB([Core],[gStyle],[],[AC_MSG_ERROR([Unable to find ROOT libraries])])
AC_LANG(C)

# Checks for custom header files.
AC_LANG(C++)
AC_CHECK_HEADER([TScanAnalysis.h],[alpide_inc_found=yes],[],[#include <stdio.h>])
AS_IF([test "x$alpide_inc_found" != "xyes"],
    AC_MSG_ERROR([Unable to find Alpide include files]))

AC_CHECK_HEADERS([DBHelpers.h utilities.h AlpideDB.h],[alpide_db_inc_found=yes],[break],[#include <stdio.h>])
AS_IF([test "x$alpide_db_inc_found" != "xyes"],
    AC_MSG_ERROR([Unable to find Alpide DB include files]))
AC_LANG(C)

# Checks for custom libraries.
AC_LANG(C++)
#AC_CHECK_LIB([alpide],[GetIddd],[],[AC_MSG_ERROR([Unable to find Alpide libraries])])
AC_HAVE_LIBRARY([alpide],[],[AC_MSG_ERROR([Unable to find Alpide libraries])])
AC_HAVE_LIBRARY([alucms],[],[AC_MSG_ERROR([Unable to find Alpide libraries])])
AC_HAVE_LIBRARY([powerboard],[],[AC_MSG_ERROR([Unable to find Alpide libraries])])
AC_HAVE_LIBRARY([mosaic],[],[AC_MSG_ERROR([Unable to find Alpide libraries])])
AC_HAVE_LIBRARY([scopecontrol],[],[AC_MSG_ERROR([Unable to find Alpide libraries])])
AC_LANG(C)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([dup2 strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
