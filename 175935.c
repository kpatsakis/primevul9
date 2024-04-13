static xmlNsPtr xmlSearchNsPrefixByHref(xmlDocPtr doc, xmlNodePtr node, const xmlChar * href)
{
	xmlNsPtr cur;
	xmlNodePtr orig = node;

	while (node) {
		if (node->type == XML_ENTITY_REF_NODE ||
		    node->type == XML_ENTITY_NODE ||
		    node->type == XML_ENTITY_DECL) {
			return NULL;
		}
		if (node->type == XML_ELEMENT_NODE) {
			cur = node->nsDef;
			while (cur != NULL) {
				if (cur->prefix && cur->href && xmlStrEqual(cur->href, href)) {
					if (xmlSearchNs(doc, node, cur->prefix) == cur) {
						return cur;
					}
				}
				cur = cur->next;
			}
			if (orig != node) {
				cur = node->ns;
				if (cur != NULL) {
					if (cur->prefix && cur->href && xmlStrEqual(cur->href, href)) {
						if (xmlSearchNs(doc, node, cur->prefix) == cur) {
							return cur;
						}
					}
				}
			}    
		}
		node = node->parent;
	}
	return NULL;
}