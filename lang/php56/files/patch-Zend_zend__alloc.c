--- Zend/zend_alloc.c.orig
+++ Zend/zend_alloc.c
@@ -91,7 +91,7 @@
 #if ZEND_DEBUG && defined(HAVE_KILL) && defined(HAVE_GETPID)
 	kill(getpid(), SIGSEGV);
 #endif
-	exit(1);
+	_exit(1);
 }
 
 /*******************/
@@ -1740,6 +1740,42 @@
 	}
 }
 
+ZEND_API void zend_mm_safe_error_ex(const char *format, ...)
+{
+	const char *error_filename;
+	uint error_lineno;
+	va_list args;
+	char buf[1024];
+	TSRMLS_FETCH();
+
+	if (zend_is_compiling(TSRMLS_C)) {
+		error_filename = zend_get_compiled_filename(TSRMLS_C);
+		error_lineno = zend_get_compiled_lineno(TSRMLS_C);
+	} else if (EG(in_execution)) {
+		error_filename = EG(active_op_array)?EG(active_op_array)->filename:NULL;
+		error_lineno = EG(opline_ptr)?(*EG(opline_ptr))->lineno:0;
+	} else {
+		error_filename = NULL;
+		error_lineno = 0;
+	}
+	if (!error_filename) {
+		error_filename = "Unknown";
+	}
+
+	va_start(args, format);
+	vsnprintf(buf, sizeof(buf), format, args);
+	va_end(args);
+
+	fprintf(stderr, "\nFatal error: %s in %s on line %d\n", buf, error_filename, error_lineno);
+
+/* See http://support.microsoft.com/kb/190351 */
+#ifdef PHP_WIN32
+	fflush(stderr);
+#endif
+
+	_exit(1);
+}
+
 static void zend_mm_safe_error(zend_mm_heap *heap,
 	const char *format,
 	size_t limit,
@@ -1749,59 +1785,15 @@
 #endif
 	size_t size)
 {
-	if (heap->reserve) {
-		_zend_mm_free_int(heap, heap->reserve ZEND_FILE_LINE_CC ZEND_FILE_LINE_EMPTY_CC);
-		heap->reserve = NULL;
-	}
-	if (heap->overflow == 0) {
-		const char *error_filename;
-		uint error_lineno;
-		TSRMLS_FETCH();
-		if (zend_is_compiling(TSRMLS_C)) {
-			error_filename = zend_get_compiled_filename(TSRMLS_C);
-			error_lineno = zend_get_compiled_lineno(TSRMLS_C);
-		} else if (EG(in_execution)) {
-			error_filename = EG(active_op_array)?EG(active_op_array)->filename:NULL;
-			error_lineno = EG(opline_ptr)?(*EG(opline_ptr))->lineno:0;
-		} else {
-			error_filename = NULL;
-			error_lineno = 0;
-		}
-		if (!error_filename) {
-			error_filename = "Unknown";
-		}
-		heap->overflow = 1;
-		zend_try {
-			zend_error_noreturn(E_ERROR,
-				format,
-				limit,
-#if ZEND_DEBUG
-				filename,
-				lineno,
-#endif
-				size);
-		} zend_catch {
-			if (heap->overflow == 2) {
-				fprintf(stderr, "\nFatal error: ");
-				fprintf(stderr,
-					format,
-					limit,
+	(void) heap;
+
+	zend_mm_safe_error_ex(format,
+		limit,
 #if ZEND_DEBUG
-					filename,
-					lineno,
-#endif
-					size);
-				fprintf(stderr, " in %s on line %d\n", error_filename, error_lineno);
-			}
-/* See http://support.microsoft.com/kb/190351 */
-#ifdef PHP_WIN32
-			fflush(stderr);
+		filename,
+		lineno,
 #endif
-		} zend_end_try();
-	} else {
-		heap->overflow = 2;
-	}
-	zend_bailout();
+		size);
 }
 
 static zend_mm_free_block *zend_mm_search_large_block(zend_mm_heap *heap, size_t true_size)
@@ -2472,7 +2464,7 @@
 	       "rm"(offset));
 
 	if (UNEXPECTED(overflow)) {
-		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
+		zend_mm_safe_error_ex("Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
 		return 0;
 	}
 	return res;
@@ -2501,7 +2493,7 @@
 
 #undef LP_SUFF
         if (UNEXPECTED(overflow)) {
-                zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
+                zend_mm_safe_error_ex("Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
                 return 0;
         }
         return res;
@@ -2522,7 +2514,7 @@
                "1"(0));
 
         if (UNEXPECTED(overflow)) {
-                zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
+                zend_mm_safe_error_ex("Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
                 return 0;
         }
         return res;
@@ -2542,7 +2534,7 @@
                "r"(offset));
 
         if (UNEXPECTED(overflow)) {
-                zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
+                zend_mm_safe_error_ex("Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
                 return 0;
         }
         return res;
@@ -2555,7 +2547,7 @@
 	zend_ulong64 res = (zend_ulong64)nmemb * (zend_ulong64)size + (zend_ulong64)offset;
 
 	if (UNEXPECTED(res > (zend_ulong64)0xFFFFFFFFL)) {
-		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
+		zend_mm_safe_error_ex("Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
 		return 0;
 	}
 	return (size_t) res;
@@ -2570,7 +2562,7 @@
 	double _delta = (double)res - _d;
 
 	if (UNEXPECTED((_d + _delta ) != _d)) {
-		zend_error_noreturn(E_ERROR, "Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
+		zend_mm_safe_error_ex("Possible integer overflow in memory allocation (%zu * %zu + %zu)", nmemb, size, offset);
 		return 0;
 	}
 	return res;
@@ -2582,7 +2574,7 @@
 {
 	size_t str_size = safe_address(nmemb, size, offset);
 	if (UNEXPECTED(str_size > INT_MAX)) {
-		zend_error_noreturn(E_ERROR, "String allocation overflow, max size is %d", INT_MAX);
+		zend_mm_safe_error_ex("String allocation overflow, max size is %d", INT_MAX);
 	}
 	return emalloc_rel(str_size);
 }
