--- ext/libxml/libxml.c.orig
+++ ext/libxml/libxml.c
@@ -421,7 +421,7 @@
 static xmlOutputBufferPtr
 php_libxml_output_buffer_create_filename(const char *URI,
                               xmlCharEncodingHandlerPtr encoder,
-                              int compression ATTRIBUTE_UNUSED)
+                              int compression)
 {
 	xmlOutputBufferPtr ret;
 	xmlURIPtr puri;
@@ -470,7 +470,7 @@
 	return 1;
 }
 
-static void _php_list_set_error_structure(xmlErrorPtr error, const char *msg)
+static void _php_list_set_error_structure(const xmlError *error, const char *msg)
 {
 	xmlError error_copy;
 	int ret;
@@ -728,7 +728,7 @@
 	va_end(args);
 }
 
-PHP_LIBXML_API void php_libxml_structured_error_handler(void *userData, xmlErrorPtr error)
+PHP_LIBXML_API void php_libxml_structured_error_handler(void *userData, const xmlError *error)
 {
 	_php_list_set_error_structure(error, NULL);
 
