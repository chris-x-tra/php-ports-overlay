--- Zend/zend_multiply.h.orig
+++ Zend/zend_multiply.h
@@ -321,7 +321,7 @@
 	size_t ret = zend_safe_address(nmemb, size, offset, &overflow);
 
 	if (UNEXPECTED(overflow)) {
-		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
+		zend_mm_safe_error_ex("Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
 		return 0;
 	}
 	return ret;
@@ -334,7 +334,7 @@
 	size_t ret = zend_safe_address(nmemb, size, offset, &overflow);
 
 	if (UNEXPECTED(overflow)) {
-		zend_error_noreturn(E_ERROR, "Possible integer overflow in %s (%zu * %zu + %zu)", message, nmemb, size, offset);
+		zend_mm_safe_error_ex("Possible integer overflow in %s (%zu * %zu + %zu)", message, nmemb, size, offset);
 		return 0;
 	}
 	return ret;
