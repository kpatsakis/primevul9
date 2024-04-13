PHP_LIBXML_API xmlNodePtr php_libxml_import_node(zval *object TSRMLS_DC)
{
	zend_class_entry *ce = NULL;
	xmlNodePtr node = NULL;
	php_libxml_func_handler *export_hnd;

	if (object->type == IS_OBJECT) {
		ce = Z_OBJCE_P(object);
		while (ce->parent != NULL) {
			ce = ce->parent;
		}
		if (zend_hash_find(&php_libxml_exports, ce->name, ce->name_length + 1, (void **) &export_hnd)  == SUCCESS) {
			node = export_hnd->export_func(object TSRMLS_CC);
		}
	}
	return node;
}