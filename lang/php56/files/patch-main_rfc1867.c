--- main/rfc1867.c.orig
+++ main/rfc1867.c
@@ -400,6 +400,14 @@
 	return 0;
 }
 
+static zend_bool php_mime_should_save_hdr(const char *name)
+{
+	return !strcasecmp(name, "Content-Disposition")
+		|| !strcasecmp(name, "Content-Type");
+}
+
+static char *php_mime_get_hdr_value(zend_llist header, char *key);
+
 /* parse headers */
 static int multipart_buffer_headers(multipart_buffer *self, zend_llist *header TSRMLS_DC)
 {
@@ -441,10 +449,15 @@
 			}
 
 			*value = '\0';
-			do { value++; } while(isspace(*value));
-
-			key = estrdup(line);
-			smart_str_appends(&buf_value, value);
+			/* Keep only headers we use, and ignore duplicates */
+			if (!php_mime_should_save_hdr(line)
+					|| php_mime_get_hdr_value(*header, line)) {
+				key = NULL;
+			} else {
+				key = estrdup(line);
+				do { value++; } while(isspace(*value));
+				smart_str_appends(&buf_value, value);
+			}
 		} else if (buf_value.c) { /* If no ':' on the line, add to previous line */
 			smart_str_appends(&buf_value, line);
 		} else {
@@ -517,11 +530,11 @@
 	return res;
 }
 
-static char *substring_conf(char *start, int len, char quote)
+static char *substring_conf(char *start, size_t len, char quote)
 {
 	char *result = emalloc(len + 1);
 	char *resp = result;
-	int i;
+	size_t i;
 
 	for (i = 0; i < len && start[i] != quote; ++i) {
 		if (start[i] == '\\' && (start[i + 1] == '\\' || (quote && start[i + 1] == quote))) {
@@ -556,7 +569,7 @@
 		while (*strend && !isspace(*strend)) {
 			++strend;
 		}
-		return substring_conf(str, strend - str, 0);
+		return substring_conf(str, (size_t)(strend - str), 0);
 	}
 }
 
@@ -696,6 +709,7 @@
 	void *event_extra_data = NULL;
 	unsigned int llen = 0;
 	int upload_cnt = INI_INT("max_file_uploads");
+	int body_parts_cnt = INI_INT("max_multipart_body_parts");
 	const zend_encoding *internal_encoding = zend_multibyte_get_internal_encoding(TSRMLS_C);
 	php_rfc1867_getword_t getword;
 	php_rfc1867_getword_conf_t getword_conf;
@@ -717,6 +731,11 @@
 		return;
 	}
 
+	if (body_parts_cnt < 0) {
+		body_parts_cnt = PG(max_input_vars) + upload_cnt;
+	}
+	int body_parts_limit = body_parts_cnt;
+
 	/* Get the boundary */
 	boundary = strstr(content_type_dup, "boundary");
 	if (!boundary) {
@@ -801,6 +820,11 @@
 			char *pair = NULL;
 			int end = 0;
 
+			if (--body_parts_cnt < 0) {
+				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Multipart body parts limit exceeded %d. To increase the limit change max_multipart_body_parts in php.ini.", body_parts_limit);
+				goto fileupload_done;
+			}
+
 			while (isspace(*cd)) {
 				++cd;
 			}
@@ -924,7 +948,10 @@
 				skip_upload = 1;
 			} else if (upload_cnt <= 0) {
 				skip_upload = 1;
-				sapi_module.sapi_error(E_WARNING, "Maximum number of allowable file uploads has been exceeded");
+				if (upload_cnt == 0) {
+					--upload_cnt;
+					sapi_module.sapi_error(E_WARNING, "Maximum number of allowable file uploads has been exceeded");
+				}
 			}
 
 			/* Return with an error if the posted data is garbled */
