xmlSetNsProp(xmlNodePtr node, xmlNsPtr ns, const xmlChar *name,
	     const xmlChar *value)
{
    xmlAttrPtr prop;

    if (ns && (ns->href == NULL))
	return(NULL);
    prop = xmlGetPropNodeInternal(node, name, (ns != NULL) ? ns->href : NULL, 0);
    if (prop != NULL) {
	/*
	* Modify the attribute's value.
	*/
	if (prop->atype == XML_ATTRIBUTE_ID) {
	    xmlRemoveID(node->doc, prop);
	    prop->atype = XML_ATTRIBUTE_ID;
	}
	if (prop->children != NULL)
	    xmlFreeNodeList(prop->children);
	prop->children = NULL;
	prop->last = NULL;
	prop->ns = ns;
	if (value != NULL) {
	    xmlNodePtr tmp;

	    if(!xmlCheckUTF8(value)) {
	        xmlTreeErr(XML_TREE_NOT_UTF8, (xmlNodePtr) node->doc,
	                   NULL);
                if (node->doc != NULL)
                    node->doc->encoding = xmlStrdup(BAD_CAST "ISO-8859-1");
	    }
	    prop->children = xmlNewDocText(node->doc, value);
	    prop->last = NULL;
	    tmp = prop->children;
	    while (tmp != NULL) {
		tmp->parent = (xmlNodePtr) prop;
		if (tmp->next == NULL)
		    prop->last = tmp;
		tmp = tmp->next;
	    }
	}
	if (prop->atype == XML_ATTRIBUTE_ID)
	    xmlAddID(NULL, node->doc, value, prop);
	return(prop);
    }
    /*
    * No equal attr found; create a new one.
    */
    return(xmlNewPropInternal(node, ns, name, value, 0));
}