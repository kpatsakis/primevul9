static int php_libxml_unregister_node(xmlNodePtr nodep TSRMLS_DC)
{
	php_libxml_node_object *wrapper;

	php_libxml_node_ptr *nodeptr = nodep->_private;

	if (nodeptr != NULL) {
		wrapper = nodeptr->_private;
		if (wrapper) {
			php_libxml_clear_object(wrapper TSRMLS_CC);
		} else {
			if (nodeptr->node != NULL && nodeptr->node->type != XML_DOCUMENT_NODE) {
				nodeptr->node->_private = NULL;
			}
			nodeptr->node = NULL;
		}
	}

	return -1;
}