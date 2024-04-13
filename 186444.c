PHP_LIBXML_API int php_libxml_decrement_doc_ref(php_libxml_node_object *object TSRMLS_DC)
{
	int ret_refcount = -1;

	if (object != NULL && object->document != NULL) {
		ret_refcount = --object->document->refcount;
		if (ret_refcount == 0) {
			if (object->document->ptr != NULL) {
				xmlFreeDoc((xmlDoc *) object->document->ptr);
			}
			if (object->document->doc_props != NULL) {
				if (object->document->doc_props->classmap) {
					zend_hash_destroy(object->document->doc_props->classmap);
					FREE_HASHTABLE(object->document->doc_props->classmap);
				}
				efree(object->document->doc_props);
			}
			efree(object->document);
			object->document = NULL;
		}
	}

	return ret_refcount;
}