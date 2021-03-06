# Check for CVODE.
#
# Sets $acx_with_cvode to yes if CVODE is found and in working order; no otherwise
#
# If CVODE is found, sets CVODE_INCLUDE, CVODE_LDFLAGS, and CVODE_LIBS
AC_DEFUN([ACX_WITH_CVODE],
[
  acx_with_cvode=no
  AC_ARG_WITH([cvode],
    [AS_HELP_STRING([--with-cvode@<:@=Install DIR@:>@], [Build with CVODE.])],
    [
      acx_with_cvode=yes
      CVODE_INCLUDE="-I$withval/include"
      CVODE_LDFLAGS="-L$withval/lib"
      CVODE_LIBS="-lsundials_cvode -lsundials_nvecserial -lm"
    ]
  )
  
  if test x"$acx_with_cvode" != xno; then
    # store the existing CPPFLAGS and LDFLAGS
    my_CPPFLAGS=$CPPFLAGS
    my_LDFLAGS=$LDFLAGS
    my_LIBS=$LIBS

    # append the CVODE information and perform the tests
    CPPFLAGS="$CVODE_INCLUDE $CPPFLAGS"
    LDFLAGS="$CVODE_LDFLAGS $LDFLAGS"

    # Check for the pressence of the necessary CVODE headers.
    AC_CHECK_HEADER([cvode/cvode.h], [],
      [AC_MSG_ERROR([Unable to find the cvode/cvode.h header file.])])
    AC_CHECK_HEADER([nvector/nvector_serial.h], [],
      [AC_MSG_ERROR([Unable to find the nvector/nvector_serial.h header file.])])
    AC_CHECK_HEADER([cvode/cvode_dense.h], [],
      [AC_MSG_ERROR([Unable to find the cvode/cvode_dense.h header file.])])
    AC_CHECK_HEADER([sundials/sundials_dense.h], [],
      [AC_MSG_ERROR([Unable to find the sundials/sundials_dense.h header file.])])
    AC_CHECK_HEADER([sundials/sundials_types.h], [],
      [AC_MSG_ERROR([Unable to find the sundials/sundials_types.h header file.])])

    # Check for the CVODE libraries
    AC_CHECK_LIB([m], [cos])
    AC_CHECK_LIB([sundials_cvode], [CVodeCreate])
    AC_CHECK_LIB([sundials_nvecserial], [main])

    # revert the CPPFLAGS and LDFLAGS
    CPPFLAGS=$my_CPPFLAGS
    LDFLAGS=$my_LDFLAGS
    LIBS=$my_LIBS
  fi

  if test x"$acx_with_cvode" != xno; then
    AC_SUBST([CVODE_INCLUDE], [$CVODE_INCLUDE])
    AC_SUBST([CVODE_LDFLAGS], [$CVODE_LDFLAGS])
    AC_SUBST([CVODE_LIBS], [$CVODE_LIBS])
  fi
])
