xmlXIncludeLoadDoc(xmlXIncludeCtxtPtr ctxt, const xmlChar *url, int nr) {
    xmlDocPtr doc;
    xmlURIPtr uri;
    xmlChar *URL;
    xmlChar *fragment = NULL;
    int i = 0;
#ifdef LIBXML_XPTR_ENABLED
    int saveFlags;
#endif

#ifdef DEBUG_XINCLUDE
    xmlGenericError(xmlGenericErrorContext, "Loading doc %s:%d\n", url, nr);
#endif
    /*
     * Check the URL and remove any fragment identifier
     */
    uri = xmlParseURI((const char *)url);
    if (uri == NULL) {
	xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	               XML_XINCLUDE_HREF_URI,
		       "invalid value URI %s\n", url);
	return(-1);
    }
    if (uri->fragment != NULL) {
	fragment = (xmlChar *) uri->fragment;
	uri->fragment = NULL;
    }
    if ((ctxt->incTab != NULL) && (ctxt->incTab[nr] != NULL) &&
        (ctxt->incTab[nr]->fragment != NULL)) {
	if (fragment != NULL) xmlFree(fragment);
	fragment = xmlStrdup(ctxt->incTab[nr]->fragment);
    }
    URL = xmlSaveUri(uri);
    xmlFreeURI(uri);
    if (URL == NULL) {
        if (ctxt->incTab != NULL)
	    xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
			   XML_XINCLUDE_HREF_URI,
			   "invalid value URI %s\n", url);
	else
	    xmlXIncludeErr(ctxt, NULL,
			   XML_XINCLUDE_HREF_URI,
			   "invalid value URI %s\n", url);
	if (fragment != NULL)
	    xmlFree(fragment);
	return(-1);
    }

    /*
     * Handling of references to the local document are done
     * directly through ctxt->doc.
     */
    if ((URL[0] == 0) || (URL[0] == '#') ||
	((ctxt->doc != NULL) && (xmlStrEqual(URL, ctxt->doc->URL)))) {
	doc = ctxt->doc;
        goto loaded;
    }

    /*
     * Prevent reloading twice the document.
     */
    for (i = 0; i < ctxt->incNr; i++) {
	if ((xmlStrEqual(URL, ctxt->incTab[i]->URI)) &&
	    (ctxt->incTab[i]->doc != NULL)) {
	    doc = ctxt->incTab[i]->doc;
#ifdef DEBUG_XINCLUDE
	    printf("Already loaded %s\n", URL);
#endif
	    goto loaded;
	}
    }

    /*
     * Load it.
     */
#ifdef DEBUG_XINCLUDE
    printf("loading %s\n", URL);
#endif
#ifdef LIBXML_XPTR_ENABLED
    /*
     * If this is an XPointer evaluation, we want to assure that
     * all entities have been resolved prior to processing the
     * referenced document
     */
    saveFlags = ctxt->parseFlags;
    if (fragment != NULL) {	/* if this is an XPointer eval */
	ctxt->parseFlags |= XML_PARSE_NOENT;
    }
#endif

    doc = xmlXIncludeParseFile(ctxt, (const char *)URL);
#ifdef LIBXML_XPTR_ENABLED
    ctxt->parseFlags = saveFlags;
#endif
    if (doc == NULL) {
	xmlFree(URL);
	if (fragment != NULL)
	    xmlFree(fragment);
	return(-1);
    }
    ctxt->incTab[nr]->doc = doc;
    /*
     * It's possible that the requested URL has been mapped to a
     * completely different location (e.g. through a catalog entry).
     * To check for this, we compare the URL with that of the doc
     * and change it if they disagree (bug 146988).
     */
   if (!xmlStrEqual(URL, doc->URL)) {
       xmlFree(URL);
       URL = xmlStrdup(doc->URL);
   }
    for (i = nr + 1; i < ctxt->incNr; i++) {
	if (xmlStrEqual(URL, ctxt->incTab[i]->URI)) {
	    ctxt->incTab[nr]->count++;
#ifdef DEBUG_XINCLUDE
	    printf("Increasing %s count since reused\n", URL);
#endif
            break;
	}
    }

    /*
     * Make sure we have all entities fixed up
     */
    xmlXIncludeMergeEntities(ctxt, ctxt->doc, doc);

    /*
     * We don't need the DTD anymore, free up space
    if (doc->intSubset != NULL) {
	xmlUnlinkNode((xmlNodePtr) doc->intSubset);
	xmlFreeNode((xmlNodePtr) doc->intSubset);
	doc->intSubset = NULL;
    }
    if (doc->extSubset != NULL) {
	xmlUnlinkNode((xmlNodePtr) doc->extSubset);
	xmlFreeNode((xmlNodePtr) doc->extSubset);
	doc->extSubset = NULL;
    }
     */
    xmlXIncludeRecurseDoc(ctxt, doc, URL);

loaded:
    if (fragment == NULL) {
	/*
	 * Add the top children list as the replacement copy.
	 */
	ctxt->incTab[nr]->inc = xmlXIncludeCopyNodeList(ctxt, ctxt->doc,
		                                        doc, doc->children);
    }
#ifdef LIBXML_XPTR_ENABLED
    else {
	/*
	 * Computes the XPointer expression and make a copy used
	 * as the replacement copy.
	 */
	xmlXPathObjectPtr xptr;
	xmlXPathContextPtr xptrctxt;
	xmlNodeSetPtr set;

	xptrctxt = xmlXPtrNewContext(doc, NULL, NULL);
	if (xptrctxt == NULL) {
	    xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	                   XML_XINCLUDE_XPTR_FAILED,
			   "could not create XPointer context\n", NULL);
	    xmlFree(URL);
	    xmlFree(fragment);
	    return(-1);
	}
	xptr = xmlXPtrEval(fragment, xptrctxt);
	if (xptr == NULL) {
	    xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	                   XML_XINCLUDE_XPTR_FAILED,
			   "XPointer evaluation failed: #%s\n",
			   fragment);
	    xmlXPathFreeContext(xptrctxt);
	    xmlFree(URL);
	    xmlFree(fragment);
	    return(-1);
	}
	switch (xptr->type) {
	    case XPATH_UNDEFINED:
	    case XPATH_BOOLEAN:
	    case XPATH_NUMBER:
	    case XPATH_STRING:
	    case XPATH_POINT:
	    case XPATH_USERS:
	    case XPATH_XSLT_TREE:
		xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
		               XML_XINCLUDE_XPTR_RESULT,
			       "XPointer is not a range: #%s\n",
			       fragment);
                xmlXPathFreeObject(xptr);
		xmlXPathFreeContext(xptrctxt);
		xmlFree(URL);
		xmlFree(fragment);
		return(-1);
	    case XPATH_NODESET:
	        if ((xptr->nodesetval == NULL) ||
		    (xptr->nodesetval->nodeNr <= 0)) {
                    xmlXPathFreeObject(xptr);
		    xmlXPathFreeContext(xptrctxt);
		    xmlFree(URL);
		    xmlFree(fragment);
		    return(-1);
		}

	    case XPATH_RANGE:
	    case XPATH_LOCATIONSET:
		break;
	}
	set = xptr->nodesetval;
	if (set != NULL) {
	    for (i = 0;i < set->nodeNr;i++) {
		if (set->nodeTab[i] == NULL)
		    continue;
		switch (set->nodeTab[i]->type) {
		    case XML_ELEMENT_NODE:
		    case XML_TEXT_NODE:
		    case XML_CDATA_SECTION_NODE:
		    case XML_ENTITY_REF_NODE:
		    case XML_ENTITY_NODE:
		    case XML_PI_NODE:
		    case XML_COMMENT_NODE:
		    case XML_DOCUMENT_NODE:
		    case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
		    case XML_DOCB_DOCUMENT_NODE:
#endif
			continue;

		    case XML_ATTRIBUTE_NODE:
			xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
			               XML_XINCLUDE_XPTR_RESULT,
				       "XPointer selects an attribute: #%s\n",
				       fragment);
			set->nodeTab[i] = NULL;
			continue;
		    case XML_NAMESPACE_DECL:
			xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
			               XML_XINCLUDE_XPTR_RESULT,
				       "XPointer selects a namespace: #%s\n",
				       fragment);
			set->nodeTab[i] = NULL;
			continue;
		    case XML_DOCUMENT_TYPE_NODE:
		    case XML_DOCUMENT_FRAG_NODE:
		    case XML_NOTATION_NODE:
		    case XML_DTD_NODE:
		    case XML_ELEMENT_DECL:
		    case XML_ATTRIBUTE_DECL:
		    case XML_ENTITY_DECL:
		    case XML_XINCLUDE_START:
		    case XML_XINCLUDE_END:
			xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
			               XML_XINCLUDE_XPTR_RESULT,
				   "XPointer selects unexpected nodes: #%s\n",
				       fragment);
			set->nodeTab[i] = NULL;
			set->nodeTab[i] = NULL;
			continue; /* for */
		}
	    }
	}
        ctxt->incTab[nr]->inc =
            xmlXIncludeCopyXPointer(ctxt, ctxt->doc, doc, xptr);
        xmlXPathFreeObject(xptr);
	xmlXPathFreeContext(xptrctxt);
	xmlFree(fragment);
    }
#endif

    /*
     * Do the xml:base fixup if needed
     */
    if ((doc != NULL) && (URL != NULL) &&
        (!(ctxt->parseFlags & XML_PARSE_NOBASEFIX)) &&
	(!(doc->parseFlags & XML_PARSE_NOBASEFIX))) {
	xmlNodePtr node;
	xmlChar *base;
	xmlChar *curBase;

	/*
	 * The base is only adjusted if "necessary", i.e. if the xinclude node
	 * has a base specified, or the URL is relative
	 */
	base = xmlGetNsProp(ctxt->incTab[nr]->ref, BAD_CAST "base",
			XML_XML_NAMESPACE);
	if (base == NULL) {
	    /*
	     * No xml:base on the xinclude node, so we check whether the
	     * URI base is different than (relative to) the context base
	     */
	    curBase = xmlBuildRelativeURI(URL, ctxt->base);
	    if (curBase == NULL) {	/* Error return */
	        xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	               XML_XINCLUDE_HREF_URI,
		       "trying to build relative URI from %s\n", URL);
	    } else {
		/* If the URI doesn't contain a slash, it's not relative */
	        if (!xmlStrchr(curBase, (xmlChar) '/'))
		    xmlFree(curBase);
		else
		    base = curBase;
	    }
	}
	if (base != NULL) {	/* Adjustment may be needed */
	    node = ctxt->incTab[nr]->inc;
	    while (node != NULL) {
		/* Only work on element nodes */
		if (node->type == XML_ELEMENT_NODE) {
		    curBase = xmlNodeGetBase(node->doc, node);
		    /* If no current base, set it */
		    if (curBase == NULL) {
			xmlNodeSetBase(node, base);
		    } else {
			/*
			 * If the current base is the same as the
			 * URL of the document, then reset it to be
			 * the specified xml:base or the relative URI
			 */
			if (xmlStrEqual(curBase, node->doc->URL)) {
			    xmlNodeSetBase(node, base);
			} else {
			    /*
			     * If the element already has an xml:base
			     * set, then relativise it if necessary
			     */
			    xmlChar *xmlBase;
			    xmlBase = xmlGetNsProp(node,
					    BAD_CAST "base",
					    XML_XML_NAMESPACE);
			    if (xmlBase != NULL) {
				xmlChar *relBase;
				relBase = xmlBuildURI(xmlBase, base);
				if (relBase == NULL) { /* error */
				    xmlXIncludeErr(ctxt,
						ctxt->incTab[nr]->ref,
						XML_XINCLUDE_HREF_URI,
					"trying to rebuild base from %s\n",
						xmlBase);
				} else {
				    xmlNodeSetBase(node, relBase);
				    xmlFree(relBase);
				}
				xmlFree(xmlBase);
			    }
			}
			xmlFree(curBase);
		    }
		}
	        node = node->next;
	    }
	    xmlFree(base);
	}
    }
    if ((nr < ctxt->incNr) && (ctxt->incTab[nr]->doc != NULL) &&
	(ctxt->incTab[nr]->count <= 1)) {
#ifdef DEBUG_XINCLUDE
        printf("freeing %s\n", ctxt->incTab[nr]->doc->URL);
#endif
	xmlFreeDoc(ctxt->incTab[nr]->doc);
	ctxt->incTab[nr]->doc = NULL;
    }
    xmlFree(URL);
    return(0);
}