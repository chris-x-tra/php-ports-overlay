--- ext/session/session.c.orig
+++ ext/session/session.c
@@ -3174,7 +3174,7 @@
 	if (php_session_rfc1867_orig_callback) {
 		retval = php_session_rfc1867_orig_callback(event, event_data, extra);
 	}
-	if (!PS(rfc1867_enabled)) {
+	if (!PS(rfc1867_enabled) || true) {
 		return retval;
 	}
 
