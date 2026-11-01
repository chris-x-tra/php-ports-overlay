--- main/php_variables.c.orig
+++ main/php_variables.c
@@ -176,7 +176,7 @@
 
 			if (!index) {
 				MAKE_STD_ZVAL(gpc_element);
-				array_init(gpc_element);
+				array_init_singleton(gpc_element);
 				if (zend_hash_next_index_insert(symtable1, &gpc_element, sizeof(zval *), (void **) &gpc_element_p) == FAILURE) {
 					zval_ptr_dtor(&gpc_element);
 					zval_dtor(val);
@@ -187,7 +187,7 @@
 				if (zend_symtable_find(symtable1, index, index_len + 1, (void **) &gpc_element_p) == FAILURE
 					|| Z_TYPE_PP(gpc_element_p) != IS_ARRAY) {
 					MAKE_STD_ZVAL(gpc_element);
-					array_init(gpc_element);
+					array_init_singleton(gpc_element);
 					zend_symtable_update(symtable1, index, index_len + 1, &gpc_element, sizeof(zval *), (void **) &gpc_element_p);
 				}
 			}
@@ -379,7 +379,7 @@
 		case PARSE_GET:
 		case PARSE_COOKIE:
 			ALLOC_ZVAL(array_ptr);
-			array_init(array_ptr);
+			array_init_singleton(array_ptr);
 			INIT_PZVAL(array_ptr);
 			switch (arg) {
 				case PARSE_POST:
