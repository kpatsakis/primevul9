PHP_LIBXML_API int php_libxml_decrement_node_ptr(php_libxml_node_object *object TSRMLS_DC)
{
	int ret_refcount = -1;
	php_libxml_node_ptr *obj_node;

	if (object != NULL && object->node != NULL) {
		obj_node = (php_libxml_node_ptr *) object->node;
		ret_refcount = --obj_node->refcount;
		if (ret_refcount == 0) {
			if (obj_node->node != NULL) {
				obj_node->node->_private = NULL;
			}
			efree(obj_node);
		} 
		object->node = NULL;
	}

	return ret_refcount;
}