# AC_LIB_RCGPARSER([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------
# Checks for the rcgparser library
AC_DEFUN([AC_LIB_RCGPARSER],
[AS_VAR_PUSHDEF([ac_lib_rcgparser], [ac_cv_lib_rcgparser])dnl
AC_CACHE_CHECK(whether the rcgparser library is available, ac_cv_lib_rcgparser,
               [AC_LANG_PUSH(C++)
                OLD_LDFLAGS="$LDFLAGS"
                LDFLAGS="$LDFLAGS -lrcgparser"
                AC_LINK_IFELSE([@%:@include <rcsslogplayer/rcgparser.hpp>
                                @%:@include <rcsslogplayer/rcgdatahandler.hpp>
                                class TestHandler
                                    : public rcss::RCGDataHandler
                                {
                                protected:
                                    void
                                    doHandleLogVersion( int ver )
                                    { m_ver = ver; }

                                    int
                                    doGetLogVersion() const
                                    { return m_ver;}

                                private:
                                    int m_ver;
                                };
                                int main()
                                {
                                    TestHandler handler;
                                    rcss::RCGParser parser( handler );
                                    return 0;
                                }],
                                [AS_VAR_SET(ac_lib_rcgparser, yes)], 
                                [AS_VAR_SET(ac_lib_rcgparser, no) 
                                 LDFLAGS="$OLD_LDFLAGS"
                                ])
                AC_LANG_POP(C++)
                ])
AS_IF([test AS_VAR_GET(ac_lib_rcgparser) = yes], [$1], [$2])
AS_VAR_POPDEF([ac_lib_rcgparser])dnl
])# AC_LIB_RCGPARSER
