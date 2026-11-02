--- config.m4.orig
+++ config.m4
@@ -83,7 +83,7 @@
     breakiterator/codepointiterator_methods.cpp"
 
   PHP_REQUIRE_CXX()
-  PHP_CXX_COMPILE_STDCXX(11, mandatory, PHP_INTL_STDCXX)
+  PHP_CXX_COMPILE_STDCXX(17, mandatory, PHP_INTL_STDCXX)
   PHP_INTL_CXX_FLAGS="$INTL_COMMON_FLAGS $PHP_INTL_STDCXX $ICU_CXXFLAGS"
   if test "$ext_shared" = "no"; then
     PHP_ADD_SOURCES(PHP_EXT_DIR(intl), $PHP_INTL_CXX_SOURCES, $PHP_INTL_CXX_FLAGS)
