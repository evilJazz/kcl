# ccache available?
*g++*|*clang* {
    system(ccache -V): CONFIG += kcl_ccache_available

    kcl_ccache_available {
        message("KCL: Using ccache for speeding up repeated builds.")
        
        # Some versions of ccache have trouble looking up the compiler binary in the PATH.
        # This especially seems to happen when running on binfmt qemu-emulated multi arch builds in Docker etc.
        # To work around this issue make sure to look up the exact binary's paths and fix it.
        
        WHICH_CCACHE = $$system(which ccache)
        message("KCL: which ccache -> $$WHICH_CCACHE")

        WHICH_CC = $$system(which $$QMAKE_CC)
        message("KCL: which $$QMAKE_CC -> $$WHICH_CC")
        WHICH_CXX = $$system(which $$QMAKE_CXX)
        message("KCL: which $$QMAKE_CXX -> $$WHICH_CXX")

        QMAKE_CC = $$WHICH_CCACHE $$WHICH_CC
        message("KCL: Setting QMAKE_CC = $$QMAKE_CC")

        QMAKE_CXX = $$WHICH_CCACHE $$WHICH_CXX
        message("KCL: Setting QMAKE_CXX = $$QMAKE_CXX")
    }
}

