--- main/SAPI.c.orig
+++ main/SAPI.c
@@ -528,18 +528,23 @@
 	}
 	if (SG(request_info).auth_user) {
 		efree(SG(request_info).auth_user);
+		SG(request_info).auth_user = NULL;
 	}
 	if (SG(request_info).auth_password) {
 		efree(SG(request_info).auth_password);
+		SG(request_info).auth_password = NULL;
 	}
 	if (SG(request_info).auth_digest) {
 		efree(SG(request_info).auth_digest);
+		SG(request_info).auth_digest = NULL;
 	}
 	if (SG(request_info).content_type_dup) {
 		efree(SG(request_info).content_type_dup);
+		SG(request_info).content_type_dup = NULL;
 	}
 	if (SG(request_info).current_user) {
 		efree(SG(request_info).current_user);
+		SG(request_info).current_user = NULL;
 	}
 	if (sapi_module.deactivate) {
 		sapi_module.deactivate(TSRMLS_C);
@@ -557,6 +562,7 @@
 	SG(callback_run) = 0;
 	if (SG(callback_func)) {
 		zval_ptr_dtor(&SG(callback_func));
+		SG(callback_func) = NULL;
 	}
 	SG(request_info).headers_read = 0;
 	SG(global_request_time) = 0;
