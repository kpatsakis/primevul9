xmlSetTreeDoc(xmlNodePtr tree, xmlDocPtr doc) {
    xmlAttrPtr prop;

    if ((tree == NULL) || (tree->type == XML_NAMESPACE_DECL))
	return;
    if (tree->doc != doc) {
	if(tree->type == XML_ELEMENT_NODE) {
	    prop = tree->properties;
	    while (prop != NULL) {
                if (prop->atype == XML_ATTRIBUTE_ID) {
                    xmlRemoveID(tree->doc, prop);
                }

		prop->doc = doc;
		xmlSetListDoc(prop->children, doc);

                /*
                 * TODO: ID attributes should be also added to the new
                 * document, but this breaks things like xmlReplaceNode.
                 * The underlying problem is that xmlRemoveID is only called
                 * if a node is destroyed, not if it's unlinked.
                 */
#if 0
                if (xmlIsID(doc, tree, prop)) {
                    xmlChar *idVal = xmlNodeListGetString(doc, prop->children,
                                                          1);
                    xmlAddID(NULL, doc, idVal, prop);
                }
#endif

		prop = prop->next;
	    }
	}
	if (tree->children != NULL)
	    xmlSetListDoc(tree->children, doc);
	tree->doc = doc;
    }
}