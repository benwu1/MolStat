AC_DEFUN([ACX_WITH_GSL],
[
  acx_with_gsl=no
  AC_ARG_WITH([gsl],
    [AS_HELP_STRING([--with-gsl@<:@=Install DIR@:>@], [Build with GSL.])],
    [
      acx_with_gsl=yes
      GSL_INCLUDE="-I$withval/include"
      GSL_LDFLAGS="-L$withval/lib"
      GSL_LIBS="-lgsl -lgslcblas -lm"
    ]
  )
  
  if test "$acx_with_gsl" != no; then
    CPPFLAGS="$GSL_INCLUDE $CPPFLAGS"
    LDFLAGS="$GSL_LDFLAGS $LDFLAGS"
    LIBS="$GSL_LIBS $LIBS"

    # Check for the pressence of the necessary GSL headers
    AC_CHECK_HEADER([gsl/gsl_rng.h], [],
      [AC_MSG_ERROR([Unable to find the gsl/gsl_rng.h header file.])])
    AC_CHECK_HEADER([gsl/gsl_randist.h], [],
      [AC_MSG_ERROR([Unable to find the gsl/gsl_randist.h header file.])])
  else
    AC_MSG_ERROR([Unable to find GSL.])
  fi
])