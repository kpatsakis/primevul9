PHP_LIBXML_API int php_libxml_increment_doc_ref(php_libxml_node_object *object, xmlDocPtr docp TSRMLS_DC)
{
	int ret_refcount = -1;

	if (object->document != NULL) {
		object->document->refcount++;
		ret_refcount = object->document->refcount;
	} else if (docp != NULL) {
		ret_refcount = 1;
		object->document = emalloc(sizeof(php_libxml_ref_obj));
		object->document->ptr = docp;
		object->document->refcount = ret_refcount;
		object->document->doc_props = NULL;
	}

	return ret_refcount;
}