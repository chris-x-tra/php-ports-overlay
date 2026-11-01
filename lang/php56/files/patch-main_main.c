--- main/main.c.orig
+++ main/main.c
@@ -628,6 +628,7 @@
 	PHP_INI_ENTRY("disable_functions",			"",			PHP_INI_SYSTEM,		NULL)
 	PHP_INI_ENTRY("disable_classes",			"",			PHP_INI_SYSTEM,		NULL)
 	PHP_INI_ENTRY("max_file_uploads",			"20",			PHP_INI_SYSTEM|PHP_INI_PERDIR,		NULL)
+	PHP_INI_ENTRY("max_multipart_body_parts",	"-1",			PHP_INI_SYSTEM|PHP_INI_PERDIR,		NULL)
 
 	STD_PHP_INI_BOOLEAN("allow_url_fopen",		"1",		PHP_INI_SYSTEM,		OnUpdateBool,		allow_url_fopen,		php_core_globals,		core_globals)
 	STD_PHP_INI_BOOLEAN("allow_url_include",	"0",		PHP_INI_SYSTEM,		OnUpdateBool,		allow_url_include,		php_core_globals,		core_globals)
@@ -1880,6 +1881,8 @@
 		for (i=0; i<NUM_TRACK_VARS; i++) {
 			if (PG(http_globals)[i]) {
 				zval_ptr_dtor(&PG(http_globals)[i]);
+				/* May be accessed before php_request_startup() */
+				PG(http_globals)[i] = NULL;
 			}
 		}
 	} zend_end_try();
@@ -1937,6 +1940,9 @@
 #ifdef HAVE_DTRACE
 	DTRACE_REQUEST_SHUTDOWN(SAFE_FILENAME(SG(request_info).path_translated), SAFE_FILENAME(SG(request_info).request_uri), (char *)SAFE_FILENAME(SG(request_info).request_method));
 #endif /* HAVE_DTRACE */
+
+	/* May be accessed before php_request_startup() */
+	PG(modules_activated) = 0;
 }
 /* }}} */
 
