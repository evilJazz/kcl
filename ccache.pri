# ccache available?
*g++*|*clang* {
    system(ccache -V): CONFIG += kcl_ccache_available

    kcl_ccache_available {
        message("KCL: Using ccache for speeding up repeated builds.")
        QMAKE_CC = ccache $$QMAKE_CC
        QMAKE_CXX = ccache $$QMAKE_CXX
    }
}

