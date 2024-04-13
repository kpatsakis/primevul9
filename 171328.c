xsltTreeAcquireStoredNs(xmlDocPtr doc,
			const xmlChar *nsName,
			const xmlChar *prefix)
{
    xmlNsPtr ns;

    if (doc == NULL)
	return (NULL);
    if (doc->oldNs != NULL)
	ns = doc->oldNs;
    else
	ns = xsltTreeEnsureXMLDecl(doc);
    if (ns == NULL)
	return (NULL);
    if (ns->next != NULL) {
	/* Reuse. */
	ns = ns->next;
	while (ns != NULL) {
	    if ((ns->prefix == NULL) != (prefix == NULL)) {
		/* NOP */
	    } else if (prefix == NULL) {
		if (xmlStrEqual(ns->href, nsName))
		    return (ns);
	    } else {
		if ((ns->prefix[0] == prefix[0]) &&
		     xmlStrEqual(ns->prefix, prefix) &&
		     xmlStrEqual(ns->href, nsName))
		    return (ns);
		
	    }
	    if (ns->next == NULL)
		break;
	    ns = ns->next;
	}
    }
    /* Create. */
    ns->next = xmlNewNs(NULL, nsName, prefix);
    return (ns->next);
}