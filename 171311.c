xsltRestoreDocumentNamespaces(xsltNsMapPtr ns, xmlDocPtr doc)
{
    if (doc == NULL)
	return(-1);
    /*
    * Revert the changes we have applied to the namespace-URIs of
    * ns-decls.
    */    
    while (ns != NULL) {
	if ((ns->doc == doc) && (ns->ns != NULL)) {
	    ns->ns->href = ns->origNsName;
	    ns->origNsName = NULL;
	    ns->ns = NULL;	    
	}
	ns = ns->next;
    }
    return(0);
}