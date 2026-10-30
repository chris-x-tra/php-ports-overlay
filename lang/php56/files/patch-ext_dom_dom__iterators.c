--- ext/dom/dom_iterators.c.orig
+++ ext/dom/dom_iterators.c
@@ -42,7 +42,7 @@
 	xmlNotation *notation;
 };
 
-static void itemHashScanner (void *payload, void *data, xmlChar *name) /* {{{ */
+static void itemHashScanner (void *payload, void *data, const xmlChar *name) /* {{{ */
 {
 	nodeIterator *priv = (nodeIterator *)data;
 
