xmlXPathFunctionLookupNS(xmlXPathContextPtr ctxt, const xmlChar *name,
			 const xmlChar *ns_uri) {
    xmlXPathFunction ret;

    if (ctxt == NULL)
	return(NULL);
    if (name == NULL)
	return(NULL);

    if (ctxt->funcLookupFunc != NULL) {
	xmlXPathFuncLookupFunc f;

	f = ctxt->funcLookupFunc;
	ret = f(ctxt->funcLookupData, name, ns_uri);
	if (ret != NULL)
	    return(ret);
    }

    if (ctxt->funcHash == NULL)
	return(NULL);

XML_IGNORE_PEDANTIC_WARNINGS
    ret = (xmlXPathFunction) xmlHashLookup2(ctxt->funcHash, name, ns_uri);
XML_POP_WARNINGS
    return(ret);
}