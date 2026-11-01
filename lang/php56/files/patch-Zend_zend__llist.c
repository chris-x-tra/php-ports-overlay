--- Zend/zend_llist.c.orig
+++ Zend/zend_llist.c
@@ -115,6 +115,8 @@
 		current = next;
 	}
 
+	l->head  = NULL;
+	l->tail  = NULL;
 	l->count = 0;
 }
 
