bin_PROGRAMS = dataComp
dataComp_SOURCES = analysislib.cpp dataComp.cpp dctrltestlib.cpp digiscanlib.cpp hiclib.cpp menulib.cpp noisescanlib.cpp powertestlib.cpp threscanlib.cpp utillib.cpp

package:
	@rm -f $(bin_PROGRAMS).tar.gz
	@tar zcpvf $(bin_PROGRAMS)_`date +%Y%m%d_%H%M%S`.tar.gz \
	     *.cpp *.h Makefile.in Makefile.am configure \
	     configure.ac AUTHORS COPYING ChangeLog INSTALL NEWS README \
	     depcomp install-sh missing
