xmlXPathRegisterVariableNS(xmlXPathContextPtr ctxt, const xmlChar *name,
			   const xmlChar *ns_uri,
			   xmlXPathObjectPtr value) {
    if (ctxt == NULL)
	return(-1);
    if (name == NULL)
	return(-1);

    if (ctxt->varHash == NULL)
	ctxt->varHash = xmlHashCreate(0);
    if (ctxt->varHash == NULL)
	return(-1);
    if (value == NULL)
        return(xmlHashRemoveEntry2(ctxt->varHash, name, ns_uri,
	                           xmlXPathFreeObjectEntry));
    return(xmlHashUpdateEntry2(ctxt->varHash, name, ns_uri,
			       (void *) value, xmlXPathFreeObjectEntry));
}