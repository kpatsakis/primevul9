xmlDOMWrapAdoptNode(xmlDOMWrapCtxtPtr ctxt,
		    xmlDocPtr sourceDoc,
		    xmlNodePtr node,
		    xmlDocPtr destDoc,
		    xmlNodePtr destParent,
		    int options)
{
    if ((node == NULL) || (node->type == XML_NAMESPACE_DECL) ||
        (destDoc == NULL) ||
	((destParent != NULL) && (destParent->doc != destDoc)))
	return(-1);
    /*
    * Check node->doc sanity.
    */
    if ((node->doc != NULL) && (sourceDoc != NULL) &&
	(node->doc != sourceDoc)) {
	/*
	* Might be an XIncluded node.
	*/
	return (-1);
    }
    if (sourceDoc == NULL)
	sourceDoc = node->doc;
    if (sourceDoc == destDoc)
	return (-1);
    switch (node->type) {
	case XML_ELEMENT_NODE:
	case XML_ATTRIBUTE_NODE:
	case XML_TEXT_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_ENTITY_REF_NODE:
	case XML_PI_NODE:
	case XML_COMMENT_NODE:
	    break;
	case XML_DOCUMENT_FRAG_NODE:
	    /* TODO: Support document-fragment-nodes. */
	    return (2);
	default:
	    return (1);
    }
    /*
    * Unlink only if @node was not already added to @destParent.
    */
    if ((node->parent != NULL) && (destParent != node->parent))
	xmlUnlinkNode(node);

    if (node->type == XML_ELEMENT_NODE) {
	    return (xmlDOMWrapAdoptBranch(ctxt, sourceDoc, node,
		    destDoc, destParent, options));
    } else if (node->type == XML_ATTRIBUTE_NODE) {
	    return (xmlDOMWrapAdoptAttr(ctxt, sourceDoc,
		(xmlAttrPtr) node, destDoc, destParent, options));
    } else {
	xmlNodePtr cur = node;
	int adoptStr = 1;

	cur->doc = destDoc;
	/*
	* Optimize string adoption.
	*/
	if ((sourceDoc != NULL) &&
	    (sourceDoc->dict == destDoc->dict))
		adoptStr = 0;
	switch (node->type) {
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
		XML_TREE_ADOPT_STR_2(node->content)
		    break;
	    case XML_ENTITY_REF_NODE:
		/*
		* Remove reference to the entitity-node.
		*/
		node->content = NULL;
		node->children = NULL;
		node->last = NULL;
		if ((destDoc->intSubset) || (destDoc->extSubset)) {
		    xmlEntityPtr ent;
		    /*
		    * Assign new entity-node if available.
		    */
		    ent = xmlGetDocEntity(destDoc, node->name);
		    if (ent != NULL) {
			node->content = ent->content;
			node->children = (xmlNodePtr) ent;
			node->last = (xmlNodePtr) ent;
		    }
		}
		XML_TREE_ADOPT_STR(node->name)
		break;
	    case XML_PI_NODE: {
		XML_TREE_ADOPT_STR(node->name)
		XML_TREE_ADOPT_STR_2(node->content)
		break;
	    }
	    default:
		break;
	}
    }
    return (0);
}