static void php_libxml_node_free_list(xmlNodePtr node TSRMLS_DC)
{
	xmlNodePtr curnode;

	if (node != NULL) {
		curnode = node;
		while (curnode != NULL) {
			node = curnode;
			switch (node->type) {
				/* Skip property freeing for the following types */
				case XML_NOTATION_NODE:
				case XML_ENTITY_DECL:
					break;
				case XML_ENTITY_REF_NODE:
					php_libxml_node_free_list((xmlNodePtr) node->properties TSRMLS_CC);
					break;
				case XML_ATTRIBUTE_NODE:
						if ((node->doc != NULL) && (((xmlAttrPtr) node)->atype == XML_ATTRIBUTE_ID)) {
							xmlRemoveID(node->doc, (xmlAttrPtr) node);
						}
				case XML_ATTRIBUTE_DECL:
				case XML_DTD_NODE:
				case XML_DOCUMENT_TYPE_NODE:
				case XML_NAMESPACE_DECL:
				case XML_TEXT_NODE:
					php_libxml_node_free_list(node->children TSRMLS_CC);
					break;
				default:
					php_libxml_node_free_list(node->children TSRMLS_CC);
					php_libxml_node_free_list((xmlNodePtr) node->properties TSRMLS_CC);
			}

			curnode = node->next;
			xmlUnlinkNode(node);
			if (php_libxml_unregister_node(node TSRMLS_CC) == 0) {
				node->doc = NULL;
			}
			php_libxml_node_free(node);
		}
	}
}