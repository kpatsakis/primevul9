xsltLREBuildEffectiveNsNodes(xsltCompilerCtxtPtr cctxt,
			     xsltStyleItemLRElementInfoPtr item,
			     xmlNodePtr elem,
			     int isLRE)
{
    xmlNsPtr ns, tmpns;
    xsltEffectiveNsPtr effNs, lastEffNs = NULL;
    int i, j, holdByElem;
    xsltPointerListPtr extElemNs = cctxt->inode->extElemNs;
    xsltPointerListPtr exclResultNs = cctxt->inode->exclResultNs;

    if ((cctxt == NULL) || (cctxt->inode == NULL) || (elem == NULL) ||
	(item == NULL) || (item->effectiveNs != NULL))
	return(-1);

    if (item->inScopeNs == NULL)    
	return(0);

    extElemNs = cctxt->inode->extElemNs;
    exclResultNs = cctxt->inode->exclResultNs;

    for (i = 0; i < item->inScopeNs->totalNumber; i++) {
	ns = item->inScopeNs->list[i];
	/*
	* Skip namespaces designated as excluded namespaces
	* -------------------------------------------------
	*
	* XSLT-20 TODO: In XSLT 2.0 we need to keep namespaces
	*  which are target namespaces of namespace-aliases
	*  regardless if designated as excluded.
	*
	* Exclude the XSLT namespace.
	*/
	if (xmlStrEqual(ns->href, XSLT_NAMESPACE))
	    goto skip_ns;

	/*
	* Apply namespace aliasing
	* ------------------------
	*
	* SPEC XSLT 2.0
	*  "- A namespace node whose string value is a literal namespace
	*     URI is not copied to the result tree.
	*   - A namespace node whose string value is a target namespace URI
	*     is copied to the result tree, whether or not the URI
	*     identifies an excluded namespace."
	* 
	* NOTE: The ns-aliasing machanism is non-cascading.
	*  (checked with Saxon, Xalan and MSXML .NET).
	* URGENT TODO: is style->nsAliases the effective list of
	*  ns-aliases, or do we need to lookup the whole
	*  import-tree?
	* TODO: Get rid of import-tree lookup.
	*/
	if (cctxt->hasNsAliases) {
	    xsltNsAliasPtr alias;
	    /*
	    * First check for being a target namespace.
	    */
	    alias = cctxt->nsAliases;
	    do {
		/*
		* TODO: Is xmlns="" handled already?
		*/
		if ((alias->targetNs != NULL) &&
		    (xmlStrEqual(alias->targetNs->href, ns->href)))
		{
		    /*
		    * Recognized as a target namespace; use it regardless
		    * if excluded otherwise.
		    */
		    goto add_effective_ns;
		}
		alias = alias->next;
	    } while (alias != NULL);

	    alias = cctxt->nsAliases;
	    do {
		/*
		* TODO: Is xmlns="" handled already?
		*/
		if ((alias->literalNs != NULL) &&
		    (xmlStrEqual(alias->literalNs->href, ns->href)))
		{
		    /*
		    * Recognized as an namespace alias; do not use it.
		    */
		    goto skip_ns;
		}
		alias = alias->next;
	    } while (alias != NULL);
	}
	
	/*
	* Exclude excluded result namespaces.
	*/
	if (exclResultNs) {
	    for (j = 0; j < exclResultNs->number; j++)
		if (xmlStrEqual(ns->href, BAD_CAST exclResultNs->items[j]))
		    goto skip_ns;
	}
	/*
	* Exclude extension-element namespaces.
	*/
	if (extElemNs) {
	    for (j = 0; j < extElemNs->number; j++)
		if (xmlStrEqual(ns->href, BAD_CAST extElemNs->items[j]))
		    goto skip_ns;
	}

add_effective_ns:
	/*
	* OPTIMIZE TODO: This information may not be needed.
	*/
	if (isLRE && (elem->nsDef != NULL)) {
	    holdByElem = 0;
	    tmpns = elem->nsDef;
	    do {
		if (tmpns == ns) {
		    holdByElem = 1;
		    break;
		}
		tmpns = tmpns->next;
	    } while (tmpns != NULL);	    
	} else
	    holdByElem = 0;
	
	
	/*
	* Add the effective namespace declaration.
	*/
	effNs = (xsltEffectiveNsPtr) xmlMalloc(sizeof(xsltEffectiveNs));
	if (effNs == NULL) {
	    xsltTransformError(NULL, cctxt->style, elem,
		"Internal error in xsltLREBuildEffectiveNs(): "
		"failed to allocate memory.\n");
	    cctxt->style->errors++;
	    return(-1);
	}
	if (cctxt->psData->effectiveNs == NULL) {
	    cctxt->psData->effectiveNs = effNs;
	    effNs->nextInStore = NULL;	 
	} else {
	    effNs->nextInStore = cctxt->psData->effectiveNs;
	    cctxt->psData->effectiveNs = effNs;
	}

	effNs->next = NULL;
	effNs->prefix = ns->prefix;
	effNs->nsName = ns->href;
	effNs->holdByElem = holdByElem;
	
	if (lastEffNs == NULL)
	    item->effectiveNs = effNs;
	else
	    lastEffNs->next = effNs;
	lastEffNs = effNs;
	
skip_ns:
	{}
    }
    return(0);
}