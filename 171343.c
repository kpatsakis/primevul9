xsltParseFindTopLevelElem(xsltCompilerCtxtPtr cctxt,
			      xmlNodePtr cur,
			      const xmlChar *name,
			      const xmlChar *namespaceURI,
			      int breakOnOtherElem,			      
			      xmlNodePtr *resultNode)
{
    if (name == NULL)
	return(-1);

    *resultNode = NULL;
    while (cur != NULL) {
	if (cur->type == XML_ELEMENT_NODE) {
	    if ((cur->ns != NULL) && (cur->name != NULL)) {
		if ((*(cur->name) == *name) &&
		    xmlStrEqual(cur->name, name) &&
		    xmlStrEqual(cur->ns->href, namespaceURI))		    
		{
		    *resultNode = cur;
		    return(1);
		}
	    }
	    if (breakOnOtherElem)
		break;
	}
	cur = cur->next;
    }
    *resultNode = cur;
    return(0);
}