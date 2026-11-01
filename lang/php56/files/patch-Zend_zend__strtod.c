--- Zend/zend_strtod.c.orig
+++ Zend/zend_strtod.c
@@ -490,7 +490,7 @@
 		rv = (Bigint *)MALLOC(sizeof(Bigint) + (x-1)*sizeof(Long));
 		if (!rv) {
 			_THREAD_PRIVATE_MUTEX_UNLOCK(dtoa_mutex);
-			zend_error(E_ERROR, "Balloc() failed to allocate memory");
+			zend_mm_safe_error_ex("Balloc() failed to allocate memory");
 		}
 		rv->k = k;
 		rv->maxwds = x;
