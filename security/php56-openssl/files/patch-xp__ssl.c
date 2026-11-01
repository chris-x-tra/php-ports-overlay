--- xp_ssl.c.orig
+++ xp_ssl.c
@@ -935,7 +935,7 @@
 static const SSL_METHOD *php_select_crypto_method(long method_value, int is_client TSRMLS_DC) /* {{{ */
 {
 	if (method_value == STREAM_CRYPTO_METHOD_SSLv2) {
-#ifndef OPENSSL_NO_SSL2
+#if !defined(OPENSSL_NO_SSL2) && OPENSSL_VERSION_NUMBER < 0x10100000L
 		return is_client ? SSLv2_client_method() : SSLv2_server_method();
 #else
 		php_error_docref(NULL TSRMLS_CC, E_WARNING,
@@ -1588,12 +1588,26 @@
 }
 /* }}} */
 
+#define PHP_SSL_MAX_VERSION_LEN 32
+
+static char *php_ssl_cipher_get_version(const SSL_CIPHER *c, char *buffer, size_t max_len) /* {{{ */
+{
+	const char *version = SSL_CIPHER_get_version(c);
+	strncpy(buffer, version, max_len);
+	if (max_len <= strlen(version)) {
+		buffer[max_len - 1] = 0;
+	}
+	return buffer;
+}
+/* }}} */
+
 static zval *capture_session_meta(SSL *ssl_handle) /* {{{ */
 {
 	zval *meta_arr;
 	char *proto_str;
 	long proto = SSL_version(ssl_handle);
 	const SSL_CIPHER *cipher = SSL_get_current_cipher(ssl_handle);
+	char version_str[PHP_SSL_MAX_VERSION_LEN];
 
 	switch (proto) {
 #if OPENSSL_VERSION_NUMBER >= 0x10001001L
@@ -1611,7 +1625,7 @@
 	add_assoc_string(meta_arr, "protocol", proto_str, 1);
 	add_assoc_string(meta_arr, "cipher_name", (char *) SSL_CIPHER_get_name(cipher), 1);
 	add_assoc_long(meta_arr, "cipher_bits", SSL_CIPHER_get_bits(cipher, NULL));
-	add_assoc_string(meta_arr, "cipher_version", SSL_CIPHER_get_version(cipher), 1);
+	add_assoc_string(meta_arr, "cipher_version", php_ssl_cipher_get_version(cipher, version_str, PHP_SSL_MAX_VERSION_LEN), 1);
 
 	return meta_arr;
 }
