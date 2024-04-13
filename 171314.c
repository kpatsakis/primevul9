xsltParseUnknownXSLTElem(xsltCompilerCtxtPtr cctxt,
			    xmlNodePtr node)
{
    if ((cctxt == NULL) || (node == NULL))
	return(-1);

    /*
    * Detection of handled content of extension instructions.
    */
    if (cctxt->inode->category == XSLT_ELEMENT_CATEGORY_EXTENSION) {
	cctxt->inode->extContentHandled = 1;
    }    
    if (cctxt->inode->forwardsCompat == 0) {	
	/*
	* We are not in forwards-compatible mode, so raise an error.
	*/
	xsltTransformError(NULL, cctxt->style, node,
	    "Unknown XSLT element '%s'.\n", node->name);
	cctxt->style->errors++;
	return(1);
    }
    /*
    * Forwards-compatible mode.
    * ------------------------
    *    
    * Parse/compile xsl:fallback elements.
    *
    * QUESTION: Do we have to raise an error if there's no xsl:fallback?
    * ANSWER: No, since in the stylesheet the fallback behaviour might
    *  also be provided by using the XSLT function "element-available".
    */
    if (cctxt->unknownItem == NULL) {
	/*
	* Create a singleton for all unknown XSLT instructions.
	*/
	cctxt->unknownItem = xsltForwardsCompatUnkownItemCreate(cctxt);
	if (cctxt->unknownItem == NULL) {
	    node->psvi = NULL;
	    return(-1);
	}
    }
    node->psvi = cctxt->unknownItem;
    if (node->children == NULL)
	return(0);
    else {
	xmlNodePtr child = node->children;

	xsltCompilerNodePush(cctxt, node);
	/*
	* Update the in-scope namespaces if needed.
	*/
	if (node->nsDef != NULL)
	    cctxt->inode->inScopeNs =
		xsltCompilerBuildInScopeNsList(cctxt, node);
	/*
	* Parse all xsl:fallback children.
	*/
	do {
	    if ((child->type == XML_ELEMENT_NODE) &&
		IS_XSLT_ELEM_FAST(child) &&
		IS_XSLT_NAME(child, "fallback"))
	    {
		cctxt->inode->curChildType = XSLT_FUNC_FALLBACK;
		xsltParseAnyXSLTElem(cctxt, child);
	    }
	    child = child->next;
	} while (child != NULL);
	
	xsltCompilerNodePop(cctxt, node);
    }
    return(0);
}