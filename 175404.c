xmlXPathRegisterFuncNS(xmlXPathContextPtr ctxt, const xmlChar *name,
		       const xmlChar *ns_uri, xmlXPathFunction f) {
    if (ctxt == NULL)
	return(-1);
    if (name == NULL)
	return(-1);

    if (ctxt->funcHash == NULL)
	ctxt->funcHash = xmlHashCreate(0);
    if (ctxt->funcHash == NULL)
	return(-1);
    if (f == NULL)
        return(xmlHashRemoveEntry2(ctxt->funcHash, name, ns_uri, NULL));
XML_IGNORE_PEDANTIC_WARNINGS
    return(xmlHashAddEntry2(ctxt->funcHash, name, ns_uri, (void *) f));
XML_POP_WARNINGS
}