AM_CPPFLAGS = -I$(top_srcdir)/src

if TRANSPORT_FITTER
noinst_LIBRARIES = libtransport_fit.a

libtransport_fit_a_SOURCES = transport_fit_module.h \
	transport_fit_module.cc \
	symmetric_resonant.h \
	symmetric_resonant.cc \
	symmetric_nonresonant.h \
	symmetric_nonresonant.cc \
	asymmetric_resonant.h \
	asymmetric_resonant.cc \
	composite_symmetric_nonresonant_background.h \
	composite_symmetric_nonresonant_background.cc \
	experiment_symmetric_nonresonant.h \
	experiment_symmetric_nonresonant.cc \
	interference.h \
	interference.cc \
	composite_interference_background.h \
	composite_interference_background.cc

# transport fit module requires GSL
libtransport_fit_a_CPPFLAGS = $(GSL_INCLUDE) $(AM_CPPFLAGS)
endif
