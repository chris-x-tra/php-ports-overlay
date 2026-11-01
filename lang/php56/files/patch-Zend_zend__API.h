--- Zend/zend_API.h.orig
+++ Zend/zend_API.h
@@ -363,10 +363,12 @@
 
 #define array_init(arg)			_array_init((arg), 0 ZEND_FILE_LINE_CC)
 #define array_init_size(arg, size) _array_init((arg), (size) ZEND_FILE_LINE_CC)
+#define array_init_singleton(arg)	_array_init_singleton((arg) ZEND_FILE_LINE_CC)
 #define object_init(arg)		_object_init((arg) ZEND_FILE_LINE_CC TSRMLS_CC)
 #define object_init_ex(arg, ce)	_object_init_ex((arg), (ce) ZEND_FILE_LINE_CC TSRMLS_CC)
 #define object_and_properties_init(arg, ce, properties)	_object_and_properties_init((arg), (ce), (properties) ZEND_FILE_LINE_CC TSRMLS_CC)
 ZEND_API int _array_init(zval *arg, uint size ZEND_FILE_LINE_DC);
+ZEND_API int _array_init_singleton(zval *arg ZEND_FILE_LINE_DC);
 ZEND_API int _object_init(zval *arg ZEND_FILE_LINE_DC TSRMLS_DC);
 ZEND_API int _object_init_ex(zval *arg, zend_class_entry *ce ZEND_FILE_LINE_DC TSRMLS_DC);
 ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *ce, HashTable *properties ZEND_FILE_LINE_DC TSRMLS_DC);
