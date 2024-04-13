xmlGetPropNodeInternal(const xmlNode *node, const xmlChar *name,
		       const xmlChar *nsName, int useDTD)
{
    xmlAttrPtr prop;

    if ((node == NULL) || (node->type != XML_ELEMENT_NODE) || (name == NULL))
	return(NULL);

    if (node->properties != NULL) {
	prop = node->properties;
	if (nsName == NULL) {
	    /*
	    * We want the attr to be in no namespace.
	    */
	    do {
		if ((prop->ns == NULL) && xmlStrEqual(prop->name, name)) {
		    return(prop);
		}
		prop = prop->next;
	    } while (prop != NULL);
	} else {
	    /*
	    * We want the attr to be in the specified namespace.
	    */
	    do {
		if ((prop->ns != NULL) && xmlStrEqual(prop->name, name) &&
		    ((prop->ns->href == nsName) ||
		     xmlStrEqual(prop->ns->href, nsName)))
		{
		    return(prop);
		}
		prop = prop->next;
	    } while (prop != NULL);
	}
    }

#ifdef LIBXML_TREE_ENABLED
    if (! useDTD)
	return(NULL);
    /*
     * Check if there is a default/fixed attribute declaration in
     * the internal or external subset.
     */
    if ((node->doc != NULL) && (node->doc->intSubset != NULL)) {
	xmlDocPtr doc = node->doc;
	xmlAttributePtr attrDecl = NULL;
	xmlChar *elemQName, *tmpstr = NULL;

	/*
	* We need the QName of the element for the DTD-lookup.
	*/
	if ((node->ns != NULL) && (node->ns->prefix != NULL)) {
	    tmpstr = xmlStrdup(node->ns->prefix);
	    tmpstr = xmlStrcat(tmpstr, BAD_CAST ":");
	    tmpstr = xmlStrcat(tmpstr, node->name);
	    if (tmpstr == NULL)
		return(NULL);
	    elemQName = tmpstr;
	} else
	    elemQName = (xmlChar *) node->name;
	if (nsName == NULL) {
	    /*
	    * The common and nice case: Attr in no namespace.
	    */
	    attrDecl = xmlGetDtdQAttrDesc(doc->intSubset,
		elemQName, name, NULL);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL)) {
		attrDecl = xmlGetDtdQAttrDesc(doc->extSubset,
		    elemQName, name, NULL);
	    }
	} else {
	    xmlNsPtr *nsList, *cur;

	    /*
	    * The ugly case: Search using the prefixes of in-scope
	    * ns-decls corresponding to @nsName.
	    */
	    nsList = xmlGetNsList(node->doc, node);
	    if (nsList == NULL) {
		if (tmpstr != NULL)
		    xmlFree(tmpstr);
		return(NULL);
	    }
	    cur = nsList;
	    while (*cur != NULL) {
		if (xmlStrEqual((*cur)->href, nsName)) {
		    attrDecl = xmlGetDtdQAttrDesc(doc->intSubset, elemQName,
			name, (*cur)->prefix);
		    if (attrDecl)
			break;
		    if (doc->extSubset != NULL) {
			attrDecl = xmlGetDtdQAttrDesc(doc->extSubset, elemQName,
			    name, (*cur)->prefix);
			if (attrDecl)
			    break;
		    }
		}
		cur++;
	    }
	    xmlFree(nsList);
	}
	if (tmpstr != NULL)
	    xmlFree(tmpstr);
	/*
	* Only default/fixed attrs are relevant.
	*/
	if ((attrDecl != NULL) && (attrDecl->defaultValue != NULL))
	    return((xmlAttrPtr) attrDecl);
    }
#endif /* LIBXML_TREE_ENABLED */
    return(NULL);
}