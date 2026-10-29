--- Zend/zend_alloc.h.orig
+++ Zend/zend_alloc.h
@@ -402,6 +402,8 @@
 size_t zend_mm_globals_size(void);
 #endif
 
+ZEND_API ZEND_COLD ZEND_NORETURN void zend_mm_safe_error_ex(const char *format, ...);
+
 END_EXTERN_C()
 
 #endif
