--- Zend/zend_API.h.orig
+++ Zend/zend_API.h
@@ -380,6 +380,7 @@
 
 #define array_init(arg)				ZVAL_ARR((arg), zend_new_array(0))
 #define array_init_size(arg, size)	ZVAL_ARR((arg), zend_new_array(size))
+#define array_init_singleton(arg)	ZVAL_ARR((arg), zend_new_array_singleton())
 ZEND_API int object_init(zval *arg);
 ZEND_API int object_init_ex(zval *arg, zend_class_entry *ce);
 ZEND_API int object_and_properties_init(zval *arg, zend_class_entry *ce, HashTable *properties);
