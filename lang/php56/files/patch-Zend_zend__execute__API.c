--- Zend/zend_execute_API.c.orig
+++ Zend/zend_execute_API.c
@@ -307,6 +307,10 @@
 		}
 	} zend_end_try();
 
+	/* may be used by zend_error before init_executor */
+	EG(error_handling) = EH_NORMAL;
+	EG(exception) = NULL;
+
 	zend_try {
 		clean_non_persistent_constants(TSRMLS_C);
 	} zend_end_try();
