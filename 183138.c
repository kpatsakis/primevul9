xmlDOMWrapRemoveNode(xmlDOMWrapCtxtPtr ctxt, xmlDocPtr doc,
		     xmlNodePtr node, int options ATTRIBUTE_UNUSED)
{
    xmlNsPtr *list = NULL;
    int sizeList, nbList, i, j;
    xmlNsPtr ns;

    if ((node == NULL) || (doc == NULL) || (node->doc != doc))
	return (-1);

    /* TODO: 0 or -1 ? */
    if (node->parent == NULL)
	return (0);

    switch (node->type) {
	case XML_TEXT_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_ENTITY_REF_NODE:
	case XML_PI_NODE:
	case XML_COMMENT_NODE:
	    xmlUnlinkNode(node);
	    return (0);
	case XML_ELEMENT_NODE:
	case XML_ATTRIBUTE_NODE:
	    break;
	default:
	    return (1);
    }
    xmlUnlinkNode(node);
    /*
    * Save out-of-scope ns-references in doc->oldNs.
    */
    do {
	switch (node->type) {
	    case XML_ELEMENT_NODE:
		if ((ctxt == NULL) && (node->nsDef != NULL)) {
		    ns = node->nsDef;
		    do {
			if (xmlDOMWrapNSNormAddNsMapItem2(&list, &sizeList,
			    &nbList, ns, ns) == -1)
			    goto internal_error;
			ns = ns->next;
		    } while (ns != NULL);
		}
		/* No break on purpose. */
	    case XML_ATTRIBUTE_NODE:
		if (node->ns != NULL) {
		    /*
		    * Find a mapping.
		    */
		    if (list != NULL) {
			for (i = 0, j = 0; i < nbList; i++, j += 2) {
			    if (node->ns == list[j]) {
				node->ns = list[++j];
				goto next_node;
			    }
			}
		    }
		    ns = NULL;
		    if (ctxt != NULL) {
			/*
			* User defined.
			*/
		    } else {
			/*
			* Add to doc's oldNs.
			*/
			ns = xmlDOMWrapStoreNs(doc, node->ns->href,
			    node->ns->prefix);
			if (ns == NULL)
			    goto internal_error;
		    }
		    if (ns != NULL) {
			/*
			* Add mapping.
			*/
			if (xmlDOMWrapNSNormAddNsMapItem2(&list, &sizeList,
			    &nbList, node->ns, ns) == -1)
			    goto internal_error;
		    }
		    node->ns = ns;
		}
		if ((node->type == XML_ELEMENT_NODE) &&
		    (node->properties != NULL)) {
		    node = (xmlNodePtr) node->properties;
		    continue;
		}
		break;
	    default:
		goto next_sibling;
	}
next_node:
	if ((node->type == XML_ELEMENT_NODE) &&
	    (node->children != NULL)) {
	    node = node->children;
	    continue;
	}
next_sibling:
	if (node == NULL)
	    break;
	if (node->next != NULL)
	    node = node->next;
	else {
	    node = node->parent;
	    goto next_sibling;
	}
    } while (node != NULL);

    if (list != NULL)
	xmlFree(list);
    return (0);

internal_error:
    if (list != NULL)
	xmlFree(list);
    return (-1);
}