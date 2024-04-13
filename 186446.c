PHP_LIBXML_API int php_libxml_increment_node_ptr(php_libxml_node_object *object, xmlNodePtr node, void *private_data TSRMLS_DC)
{
	int ret_refcount = -1;

	if (object != NULL && node != NULL) {
		if (object->node != NULL) {
			if (object->node->node == node) {
				return object->node->refcount;
			} else {
				php_libxml_decrement_node_ptr(object TSRMLS_CC);
			}
		}
		if (node->_private != NULL) {
			object->node = node->_private;
			ret_refcount = ++object->node->refcount;
			/* Only dom uses _private */
			if (object->node->_private == NULL) {
				object->node->_private = private_data;
			}
		} else {
			ret_refcount = 1;
			object->node = emalloc(sizeof(php_libxml_node_ptr));
			object->node->node = node;
			object->node->refcount = 1;
			object->node->_private = private_data;
			node->_private = object->node;
		}
	}

	return ret_refcount;
}