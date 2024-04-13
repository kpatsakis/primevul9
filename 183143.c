xmlDOMWrapAdoptAttr(xmlDOMWrapCtxtPtr ctxt,
		    xmlDocPtr sourceDoc,
		    xmlAttrPtr attr,
		    xmlDocPtr destDoc,
		    xmlNodePtr destParent,
		    int options ATTRIBUTE_UNUSED)
{
    xmlNodePtr cur;
    int adoptStr = 1;

    if ((attr == NULL) || (destDoc == NULL))
	return (-1);

    attr->doc = destDoc;
    if (attr->ns != NULL) {
	xmlNsPtr ns = NULL;

	if (ctxt != NULL) {
	    /* TODO: User defined. */
	}
	/* XML Namespace. */
	if (IS_STR_XML(attr->ns->prefix)) {
	    ns = xmlTreeEnsureXMLDecl(destDoc);
	} else if (destParent == NULL) {
	    /*
	    * Store in @destDoc->oldNs.
	    */
	    ns = xmlDOMWrapStoreNs(destDoc, attr->ns->href, attr->ns->prefix);
	} else {
	    /*
	    * Declare on @destParent.
	    */
	    if (xmlSearchNsByNamespaceStrict(destDoc, destParent, attr->ns->href,
		&ns, 1) == -1)
		goto internal_error;
	    if (ns == NULL) {
		ns = xmlDOMWrapNSNormDeclareNsForced(destDoc, destParent,
		    attr->ns->href, attr->ns->prefix, 1);
	    }
	}
	if (ns == NULL)
	    goto internal_error;
	attr->ns = ns;
    }

    XML_TREE_ADOPT_STR(attr->name);
    attr->atype = 0;
    attr->psvi = NULL;
    /*
    * Walk content.
    */
    if (attr->children == NULL)
	return (0);
    cur = attr->children;
    if ((cur != NULL) && (cur->type == XML_NAMESPACE_DECL))
        goto internal_error;
    while (cur != NULL) {
	cur->doc = destDoc;
	switch (cur->type) {
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
		XML_TREE_ADOPT_STR_2(cur->content)
		break;
	    case XML_ENTITY_REF_NODE:
		/*
		* Remove reference to the entitity-node.
		*/
		cur->content = NULL;
		cur->children = NULL;
		cur->last = NULL;
		if ((destDoc->intSubset) || (destDoc->extSubset)) {
		    xmlEntityPtr ent;
		    /*
		    * Assign new entity-node if available.
		    */
		    ent = xmlGetDocEntity(destDoc, cur->name);
		    if (ent != NULL) {
			cur->content = ent->content;
			cur->children = (xmlNodePtr) ent;
			cur->last = (xmlNodePtr) ent;
		    }
		}
		break;
	    default:
		break;
	}
	if (cur->children != NULL) {
	    cur = cur->children;
	    continue;
	}
next_sibling:
	if (cur == (xmlNodePtr) attr)
	    break;
	if (cur->next != NULL)
	    cur = cur->next;
	else {
	    cur = cur->parent;
	    goto next_sibling;
	}
    }
    return (0);
internal_error:
    return (-1);
}