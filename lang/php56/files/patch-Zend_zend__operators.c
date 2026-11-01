--- Zend/zend_operators.c.orig
+++ Zend/zend_operators.c
@@ -1400,7 +1400,7 @@
 	char *buf;
 
 	if (UNEXPECTED(length < 0)) {
-		zend_error(E_ERROR, "String size overflow");
+		zend_mm_safe_error_ex("String size overflow");
 	}
 
 	buf = str_erealloc(Z_STRVAL_P(op1), length + 1);
@@ -1419,7 +1419,7 @@
 	char *buf;
 
 	if (UNEXPECTED(length < 0)) {
-		zend_error(E_ERROR, "String size overflow");
+		zend_mm_safe_error_ex("String size overflow");
 	}
 
 	buf = str_erealloc(Z_STRVAL_P(op1), length + 1);
@@ -1465,7 +1465,7 @@
 		if (Z_STRLEN_P(result) < 0 || (int) (Z_STRLEN_P(op1) + Z_STRLEN_P(op2)) < 0) {
 			efree(Z_STRVAL_P(result));
 			ZVAL_EMPTY_STRING(result);
-			zend_error(E_ERROR, "String size overflow");
+			zend_mm_safe_error_ex("String size overflow");
 		}
 
 		Z_STRVAL_P(result) = safe_erealloc(Z_STRVAL_P(result), res_len, 1, 1);
@@ -1478,7 +1478,7 @@
 		char *buf;
 
 		if (Z_STRLEN_P(op1) < 0 || Z_STRLEN_P(op2) < 0 || (int) (Z_STRLEN_P(op1) + Z_STRLEN_P(op2)) < 0) {
-			zend_error(E_ERROR, "String size overflow");
+			zend_mm_safe_error_ex("String size overflow");
 		}
 		buf = (char *) safe_emalloc(length, 1, 1);
 
