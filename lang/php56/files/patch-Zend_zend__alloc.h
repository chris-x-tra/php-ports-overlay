--- Zend/zend_alloc.h.orig
+++ Zend/zend_alloc.h
@@ -57,6 +57,12 @@
 ZEND_API void *_emalloc(size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE(1);
 ZEND_API void *_safe_emalloc(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
 ZEND_API void *_safe_emalloc_string(size_t nmemb, size_t size, size_t offset ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC;
+
+#if defined(__GNUC__) && __GNUC__ >= 3 && !defined(__INTEL_COMPILER) && !defined(DARWIN) && !defined(__hpux) && !defined(_AIX) && !defined(__osf__)
+ZEND_API void zend_mm_safe_error_ex(const char *format, ...) __attribute__ ((noreturn));
+#else
+ZEND_API void zend_mm_safe_error_ex(const char *format, ...);
+#endif
 ZEND_API void *_safe_malloc(size_t nmemb, size_t size, size_t offset) ZEND_ATTRIBUTE_MALLOC;
 ZEND_API void _efree(void *ptr ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC);
 ZEND_API void *_ecalloc(size_t nmemb, size_t size ZEND_FILE_LINE_DC ZEND_FILE_LINE_ORIG_DC) ZEND_ATTRIBUTE_MALLOC ZEND_ATTRIBUTE_ALLOC_SIZE2(1,2);
