xmlXPathRegisterNs(xmlXPathContextPtr ctxt, const xmlChar *prefix,
			   const xmlChar *ns_uri) {
    if (ctxt == NULL)
	return(-1);
    if (prefix == NULL)
	return(-1);
    if (prefix[0] == 0)
	return(-1);

    if (ctxt->nsHash == NULL)
	ctxt->nsHash = xmlHashCreate(10);
    if (ctxt->nsHash == NULL)
	return(-1);
    if (ns_uri == NULL)
        return(xmlHashRemoveEntry(ctxt->nsHash, prefix,
	                          xmlHashDefaultDeallocator));
    return(xmlHashUpdateEntry(ctxt->nsHash, prefix, (void *) xmlStrdup(ns_uri),
			      xmlHashDefaultDeallocator));
}