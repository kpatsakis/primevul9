xsltTreeEnsureXMLDecl(xmlDocPtr doc)
{
    if (doc == NULL)
	return (NULL);
    if (doc->oldNs != NULL)
	return (doc->oldNs);
    {
	xmlNsPtr ns;
	ns = (xmlNsPtr) xmlMalloc(sizeof(xmlNs));
	if (ns == NULL) {
	    xmlGenericError(xmlGenericErrorContext,
		"xsltTreeEnsureXMLDecl: Failed to allocate "
		"the XML namespace.\n");	
	    return (NULL);
	}
	memset(ns, 0, sizeof(xmlNs));
	ns->type = XML_LOCAL_NAMESPACE;
	/*
	* URGENT TODO: revisit this.
	*/
#ifdef LIBXML_NAMESPACE_DICT
	if (doc->dict)
	    ns->href = xmlDictLookup(doc->dict, XML_XML_NAMESPACE, -1);
	else
	    ns->href = xmlStrdup(XML_XML_NAMESPACE);
#else
	ns->href = xmlStrdup(XML_XML_NAMESPACE); 
#endif
	ns->prefix = xmlStrdup((const xmlChar *)"xml");
	doc->oldNs = ns;
	return (ns);
    }
}