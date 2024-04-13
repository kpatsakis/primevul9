xsltTestCompMatchDirect(xsltTransformContextPtr ctxt, xsltCompMatchPtr comp,
	                xmlNodePtr node, xmlNsPtr *nsList, int nsNr) {
    xsltStepOpPtr sel = NULL;
    xmlDocPtr prevdoc;
    xmlDocPtr doc;
    xmlXPathObjectPtr list;
    int ix, j;
    int nocache = 0;
    int isRVT;

    doc = node->doc;
    if (XSLT_IS_RES_TREE_FRAG(doc))
	isRVT = 1;
    else
	isRVT = 0;
    sel = &comp->steps[0]; /* store extra in first step arbitrarily */

    prevdoc = (xmlDocPtr)
	XSLT_RUNTIME_EXTRA(ctxt, sel->previousExtra, ptr);
    ix = XSLT_RUNTIME_EXTRA(ctxt, sel->indexExtra, ival);
    list = (xmlXPathObjectPtr)
	XSLT_RUNTIME_EXTRA_LST(ctxt, sel->lenExtra);
    
    if ((list == NULL) || (prevdoc != doc)) {
	xmlXPathObjectPtr newlist;
	xmlNodePtr parent = node->parent;
	xmlDocPtr olddoc;
	xmlNodePtr oldnode;
	int oldNsNr, oldContextSize, oldProximityPosition;
	xmlNsPtr *oldNamespaces;

	oldnode = ctxt->xpathCtxt->node;
	olddoc = ctxt->xpathCtxt->doc;
	oldNsNr = ctxt->xpathCtxt->nsNr;
	oldNamespaces = ctxt->xpathCtxt->namespaces;
	oldContextSize = ctxt->xpathCtxt->contextSize;
	oldProximityPosition = ctxt->xpathCtxt->proximityPosition;
	ctxt->xpathCtxt->node = node;
	ctxt->xpathCtxt->doc = doc;
	ctxt->xpathCtxt->namespaces = nsList;
	ctxt->xpathCtxt->nsNr = nsNr;
	newlist = xmlXPathEval(comp->pattern, ctxt->xpathCtxt);
	ctxt->xpathCtxt->node = oldnode;
	ctxt->xpathCtxt->doc = olddoc;
	ctxt->xpathCtxt->namespaces = oldNamespaces;
	ctxt->xpathCtxt->nsNr = oldNsNr;
	ctxt->xpathCtxt->contextSize = oldContextSize;
	ctxt->xpathCtxt->proximityPosition = oldProximityPosition;
	if (newlist == NULL)
	    return(-1);
	if (newlist->type != XPATH_NODESET) {
	    xmlXPathFreeObject(newlist);
	    return(-1);
	}
	ix = 0;

	if ((parent == NULL) || (node->doc == NULL) || isRVT)
	    nocache = 1;
	
	if (nocache == 0) {
	    if (list != NULL)
		xmlXPathFreeObject(list);
	    list = newlist;

	    XSLT_RUNTIME_EXTRA_LST(ctxt, sel->lenExtra) =
		(void *) list;
	    XSLT_RUNTIME_EXTRA(ctxt, sel->previousExtra, ptr) =
		(void *) doc;
	    XSLT_RUNTIME_EXTRA(ctxt, sel->indexExtra, ival) =
		0;
	    XSLT_RUNTIME_EXTRA_FREE(ctxt, sel->lenExtra) =
		(xmlFreeFunc) xmlXPathFreeObject;
	} else
	    list = newlist;
    }
    if ((list->nodesetval == NULL) ||
	(list->nodesetval->nodeNr <= 0)) {
	if (nocache == 1)
	    xmlXPathFreeObject(list);
	return(0);
    }
    /* TODO: store the index and use it for the scan */
    if (ix == 0) {
	for (j = 0;j < list->nodesetval->nodeNr;j++) {
	    if (list->nodesetval->nodeTab[j] == node) {
		if (nocache == 1)
		    xmlXPathFreeObject(list);
		return(1);
	    }
	}
    } else {
    }
    if (nocache == 1)
	xmlXPathFreeObject(list);
    return(0);
}