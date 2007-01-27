dnl @synopsis AX_PROG_DOXYGEN([MINIMUM-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
dnl @summary  This macro searches for Doxygen program.
dnl @category InstalledPackages
dnl
dnl This macro tests if Doxygen is installed in the system.
dnl It can be controlled by calling ./configure with or without
dnl --with-doxygen option:
dnl
dnl 1) Search for doxygen executable in PATH if:
dnl    a) ./configure is called without --with-doxygen
dnl    b) --with-doxygen is specified but without any value
dnl    c) --with-doxygen=yes is specified 
dnl 2) Search for doxygen executable in diven directory:
dnl    if --with-doxygen=DIR is specified.
dnl
dnl All parameters are optional when calling this macro.
dnl If MINIMUM-VERSION is not provided, 1.3.0 is set as default.
dnl
dnl Macro calls:
dnl
dnl     AC_SUBS(DOXYGEN)
dnl     AC_SUBS(DOXYGEN_VERSION)
dnl
dnl Macro sets:
dnl
dnl     DOXYGEN - path to doxygen executable
dnl     DOXYGEN_VERSION - version of detected Doxygen package
dnl
dnl @author Mateusz Loskot <mateusz@loskot.net>
dnl @version 2006-06-09
dnl @license AllPermissive
dnl
AC_DEFUN([AX_PROG_DOXYGEN],
[
    AC_MSG_CHECKING([if Doxygen is required])

    AC_ARG_WITH(
        [doxygen],
        AC_HELP_STRING(
            [--with-doxygen],
            [use doxygen (default is YES), specify the directory with doxygen program (optional)]
        ),
        [
            if test "$withval" != "no"; then
                want_doxygen="yes"

                if test "$withval" != "yes"; then
                    ac_doxygen_dir="$withval"
                fi
            else
                want_doxygen="no"
            fi
        ],
        [want_doxygen="yes"]
    )

    AC_MSG_RESULT([$want_doxygen])

    if test "x$want_doxygen" = "xyes"; then
        if test -z "$ac_doxygen_dir"; then
            AC_PATH_PROG([DOXYGEN], [doxygen], [], [])
        else
            AC_PATH_PROG([DOXYGEN], [doxygen], [], $ac_doxygen_dir)
        fi

        AC_SUBST([DOXYGEN])
        
        if test -z "$DOXYGEN"; then
            # ACTION-IF-NOT-FOUND
            ifelse([$3], ,
                   AC_MSG_ERROR([Doxygen executable is required but can not be found!
                   You can download Doxygen from http://www.doxygen.org]),
                   [$3])
        else
            # Check MINIMUM-VERSION
            doxygen_version_req=ifelse([$1], , 1.3.0, [$1])
            doxygen_version_req_major=`expr $doxygen_version_req : '\([[0-9]]*\)'`
            doxygen_version_req_minor=`expr $doxygen_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
            doxygen_version_req_micro=`expr $doxygen_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
            
            doxygen_version=`$DOXYGEN --version`
            doxygen_version_major=`expr $doxygen_version : '\([[0-9]]*\)'`
            doxygen_version_minor=`expr $doxygen_version : '[[0-9]]*\.\([[0-9]]*\)'`
            doxygen_version_micro=`expr $doxygen_version : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`

            WANT_DOXYGEN_VERSION=`expr $doxygen_version_req_major \* 100000 \+  $doxygen_version_req_minor \* 100 \+ $doxygen_version_req_micro`
            FOUND_DOXYGEN_VERSION=`expr $doxygen_version_major \* 100000 \+  $doxygen_version_minor \* 100 \+ $doxygen_version_micro`

            if test $WANT_DOXYGEN_VERESION >= $FOUND_DOXYGEN_VERSION; then
                DOXYGEN_VERSION=$FOUND_DOXYGEN_VERSION
            else
                # Required Doxygen not found
                AC_MSG_ERROR([Doxygen $FOUND_DOXYGEN_VERSION found! Required version is $WANTED_DOXYGEN_VERSION])
            fi

            # ACTION-IF-FOUND
            ifelse([$2], , :, [$2])
        fi
    fi

])
