static int php_libxml_clear_object(php_libxml_node_object *object TSRMLS_DC)
{
	if (object->properties) {
		object->properties = NULL;
	}
	php_libxml_decrement_node_ptr(object TSRMLS_CC);
	return php_libxml_decrement_doc_ref(object TSRMLS_CC);
}