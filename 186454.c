PHP_LIBXML_API void php_libxml_node_free_resource(xmlNodePtr node TSRMLS_DC)
{
	if (!node) {
		return;
	}

	switch (node->type) {
		case XML_DOCUMENT_NODE:
		case XML_HTML_DOCUMENT_NODE:
			break;
		default:
			if (node->parent == NULL || node->type == XML_NAMESPACE_DECL) {
				php_libxml_node_free_list((xmlNodePtr) node->children TSRMLS_CC);
				switch (node->type) {
					/* Skip property freeing for the following types */
					case XML_ATTRIBUTE_DECL:
					case XML_DTD_NODE:
					case XML_DOCUMENT_TYPE_NODE:
					case XML_ENTITY_DECL:
					case XML_ATTRIBUTE_NODE:
					case XML_NAMESPACE_DECL:
					case XML_TEXT_NODE:
						break;
					default:
						php_libxml_node_free_list((xmlNodePtr) node->properties TSRMLS_CC);
				}
				if (php_libxml_unregister_node(node TSRMLS_CC) == 0) {
					node->doc = NULL;
				}
				php_libxml_node_free(node);
			} else {
				php_libxml_unregister_node(node TSRMLS_CC);
			}
	}
}