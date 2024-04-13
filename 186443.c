PHP_LIBXML_API void php_libxml_node_decrement_resource(php_libxml_node_object *object TSRMLS_DC)
{
	int ret_refcount = -1;
	xmlNodePtr nodep;
	php_libxml_node_ptr *obj_node;

	if (object != NULL && object->node != NULL) {
		obj_node = (php_libxml_node_ptr *) object->node;
		nodep = object->node->node;
		ret_refcount = php_libxml_decrement_node_ptr(object TSRMLS_CC);
		if (ret_refcount == 0) {
			php_libxml_node_free_resource(nodep TSRMLS_CC);
		} else {
			if (obj_node && object == obj_node->_private) {
				obj_node->_private = NULL;
			}
		}
	}
	if (object != NULL && object->document != NULL) {
		/* Safe to call as if the resource were freed then doc pointer is NULL */
		php_libxml_decrement_doc_ref(object TSRMLS_CC);
	}
}