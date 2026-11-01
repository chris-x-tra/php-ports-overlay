--- openssl.c.orig
+++ openssl.c
@@ -579,6 +579,13 @@
 #endif
 
 /* {{{ OpenSSL compatibility functions and macros */
+#if OPENSSL_VERSION_NUMBER >= 0x30000000L && !defined(LIBRESSL_VERSION_NUMBER)
+/* OpenSSL 3 returns const pointers here; the code expects non-const ones */
+#define EVP_PKEY_get0_RSA(_pkey) ((RSA *) EVP_PKEY_get0_RSA(_pkey))
+#define EVP_PKEY_get0_DSA(_pkey) ((DSA *) EVP_PKEY_get0_DSA(_pkey))
+#define EVP_PKEY_get0_DH(_pkey) ((DH *) EVP_PKEY_get0_DH(_pkey))
+#define EVP_PKEY_get0_EC_KEY(_pkey) ((EC_KEY *) EVP_PKEY_get0_EC_KEY(_pkey))
+#endif
 #if PHP_OPENSSL_API_VERSION < 0x10100
 #define EVP_PKEY_get0_RSA(_pkey) _pkey->pkey.rsa
 #define EVP_PKEY_get0_DH(_pkey) _pkey->pkey.dh
@@ -1517,7 +1524,9 @@
 	REGISTER_LONG_CONSTANT("PKCS7_NOSIGS", PKCS7_NOSIGS, CONST_CS|CONST_PERSISTENT);
 
 	REGISTER_LONG_CONSTANT("OPENSSL_PKCS1_PADDING", RSA_PKCS1_PADDING, CONST_CS|CONST_PERSISTENT);
+#ifdef RSA_SSLV23_PADDING
 	REGISTER_LONG_CONSTANT("OPENSSL_SSLV23_PADDING", RSA_SSLV23_PADDING, CONST_CS|CONST_PERSISTENT);
+#endif
 	REGISTER_LONG_CONSTANT("OPENSSL_NO_PADDING", RSA_NO_PADDING, CONST_CS|CONST_PERSISTENT);
 	REGISTER_LONG_CONSTANT("OPENSSL_PKCS1_OAEP_PADDING", RSA_PKCS1_OAEP_PADDING, CONST_CS|CONST_PERSISTENT);
 
