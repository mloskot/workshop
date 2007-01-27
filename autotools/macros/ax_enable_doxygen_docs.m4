dnl @synopsis AX_ENABLE_DOXYGEN_DOCS([OUTPUT-FORMATS])
dnl @summary  This macro enables doxygen-docs target for make
dnl @category
dnl
dnl Macros requirements:
dnl
dnl     AX_PROG_DOXYGEN
dnl
dnl Macro calls:
dnl
dnl     AC_SUBST(DOXYGEN_DOCS)
dnl
dnl Macro sets:
dnl
dnl     DOXYGEN_DOCS - list of Doxygen output formats
dnl
dnl @author Mateusz Loskot <mateusz@loskot.net>
dnl @version 2006-06-09
dnl @license AllPermissive
dnl
AC_DEFUN([AX_ENABLE_DOXYGEN_DOCS],
[
    AC_ARG_ENABLE([doxygen-docs], 
        AC_HELP_STRING([--enable-doxygen-docs],
            [enable 'doxygen-docs' target to build Doxygen documentation (requires Doxygen)]
        ),
        [
            if test "$enableval" = "yes"; then
                AX_PROG_DOXYGEN([],
                    [AC_MSG_RESULT([Makefile target 'doxygen-html' has been enabled])],
                    [AC_MSG_WARN([Doxygen not found, so Doxygen documentation can not be enabled!
                        You can download latest Doxygen from http://www.doxygen.org])]
                )

                DOXYGEN_HTML="no"
                DOXYGEN_LATEX="no"
                DOXYGEN_MAN="no"
                DOXYGEN_RTF="no"
                DOXYGEN_XML="no"

                for fmt in $1
                do
                    FMT="`echo $fmt|tr [a-z] [A-Z]`"
                    eval "DOXYGEN_$FMT=\"yes\""
               done
            fi
            
            AC_SUBST([DOXYGEN_HTML])
            AC_SUBST([DOXYGEN_LATEX])
            AC_SUBST([DOXYGEN_MAN])
            AC_SUBST([DOXYGEN_RTF])
            AC_SUBST([DOXYGEN_XML])
        ],
    )
])
