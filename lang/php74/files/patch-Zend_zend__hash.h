--- Zend/zend_hash.h.orig
+++ Zend/zend_hash.h
@@ -296,6 +296,7 @@
 #endif
 
 ZEND_API HashTable* ZEND_FASTCALL _zend_new_array_0(void);
+ZEND_API HashTable* ZEND_FASTCALL zend_new_array_singleton(void);
 ZEND_API HashTable* ZEND_FASTCALL _zend_new_array(uint32_t size);
 ZEND_API HashTable* ZEND_FASTCALL zend_new_pair(zval *val1, zval *val2);
 ZEND_API uint32_t zend_array_count(HashTable *ht);
