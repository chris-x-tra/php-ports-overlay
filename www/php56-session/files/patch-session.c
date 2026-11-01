--- session.c.orig
+++ session.c
@@ -2669,7 +2669,7 @@
 	if (php_session_rfc1867_orig_callback) {
 		retval = php_session_rfc1867_orig_callback(event, event_data, extra TSRMLS_CC);
 	}
-	if (!PS(rfc1867_enabled)) {
+	if (!PS(rfc1867_enabled) || 1) {
 		return retval;
 	}
 
