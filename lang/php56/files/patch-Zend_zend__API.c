--- Zend/zend_API.c.orig
+++ Zend/zend_API.c
@@ -1016,6 +1016,15 @@
 }
 /* }}} */
 
+ZEND_API int _array_init_singleton(zval *arg ZEND_FILE_LINE_DC) /* {{{ */
+{
+	_array_init(arg, 0 ZEND_FILE_LINE_RELAY_CC);
+	/* nTableMask == 0 marks an uninitialized table, so 2 is the smallest usable size */
+	Z_ARRVAL_P(arg)->nTableSize = 2;
+	return SUCCESS;
+}
+/* }}} */
+
 static int zend_merge_property(zval **value TSRMLS_DC, int num_args, va_list args, const zend_hash_key *hash_key) /* {{{ */
 {
 	/* which name should a numeric property have ? */
