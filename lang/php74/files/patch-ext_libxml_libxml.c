--- ext/libxml/libxml.c.orig
+++ ext/libxml/libxml.c
@@ -476,7 +476,7 @@
 static xmlOutputBufferPtr
 php_libxml_output_buffer_create_filename(const char *URI,
                               xmlCharEncodingHandlerPtr encoder,
-                              int compression ATTRIBUTE_UNUSED)
+                              int compression)
 {
 	xmlOutputBufferPtr ret;
 	xmlURIPtr puri;
@@ -530,7 +530,7 @@
 	return 1;
 }
 
-static void _php_list_set_error_structure(xmlErrorPtr error, const char *msg)
+static void _php_list_set_error_structure(const xmlError *error, const char *msg)
 {
 	xmlError error_copy;
 	int ret;
@@ -782,7 +782,7 @@
 	va_end(args);
 }
 
-PHP_LIBXML_API void php_libxml_structured_error_handler(void *userData, xmlErrorPtr error)
+PHP_LIBXML_API void php_libxml_structured_error_handler(void *userData, const xmlError *error)
 {
 	_php_list_set_error_structure(error, NULL);
 
@@ -1061,7 +1061,7 @@
    Retrieve last error from libxml */
 static PHP_FUNCTION(libxml_get_last_error)
 {
-	xmlErrorPtr error;
+	const xmlError *error;
 
 	error = xmlGetLastError();
 
