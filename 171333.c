xsltGatherNamespaces(xsltStylesheetPtr style) {
    xmlNodePtr cur;
    const xmlChar *URI;

    if (style == NULL)
        return;
    /* 
     * TODO: basically if the stylesheet uses the same prefix for different
     *       patterns, well they may be in problem, hopefully they will get
     *       a warning first.
     */
    /*
    * TODO: Eliminate the use of the hash for XPath expressions.
    *   An expression should be evaluated in the context of the in-scope
    *   namespaces; eliminate the restriction of an XML document to contain
    *   no duplicate prefixes for different namespace names.
    * 
    */
    cur = xmlDocGetRootElement(style->doc);
    while (cur != NULL) {
	if (cur->type == XML_ELEMENT_NODE) {
	    xmlNsPtr ns = cur->nsDef;
	    while (ns != NULL) {
		if (ns->prefix != NULL) {
		    if (style->nsHash == NULL) {
			style->nsHash = xmlHashCreate(10);
			if (style->nsHash == NULL) {
			    xsltTransformError(NULL, style, cur,
		 "xsltGatherNamespaces: failed to create hash table\n");
			    style->errors++;
			    return;
			}
		    }
		    URI = xmlHashLookup(style->nsHash, ns->prefix);
		    if ((URI != NULL) && (!xmlStrEqual(URI, ns->href))) {
			xsltTransformError(NULL, style, cur,
	     "Namespaces prefix %s used for multiple namespaces\n",ns->prefix);
			style->warnings++;
		    } else if (URI == NULL) {
			xmlHashUpdateEntry(style->nsHash, ns->prefix,
			    (void *) ns->href, (xmlHashDeallocator)xmlFree);

#ifdef WITH_XSLT_DEBUG_PARSING
			xsltGenericDebug(xsltGenericDebugContext,
		 "Added namespace: %s mapped to %s\n", ns->prefix, ns->href);
#endif
		    }
		}
		ns = ns->next;
	    }
	}

	/*
	 * Skip to next node
	 */
	if (cur->children != NULL) {
	    if (cur->children->type != XML_ENTITY_DECL) {
		cur = cur->children;
		continue;
	    }
	}
	if (cur->next != NULL) {
	    cur = cur->next;
	    continue;
	}
	
	do {
	    cur = cur->parent;
	    if (cur == NULL)
		break;
	    if (cur == (xmlNodePtr) style->doc) {
		cur = NULL;
		break;
	    }
	    if (cur->next != NULL) {
		cur = cur->next;
		break;
	    }
	} while (cur != NULL);
    }
}