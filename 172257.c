xsltCopyText(xsltTransformContextPtr ctxt, xmlNodePtr target,
	     xmlNodePtr cur, int interned)
{
    xmlNodePtr copy;

    if ((cur->type != XML_TEXT_NODE) &&
	(cur->type != XML_CDATA_SECTION_NODE))
	return(NULL);
    if (cur->content == NULL)
	return(NULL);

#ifdef WITH_XSLT_DEBUG_PROCESS
    if (cur->type == XML_CDATA_SECTION_NODE) {
	XSLT_TRACE(ctxt,XSLT_TRACE_COPY_TEXT,xsltGenericDebug(xsltGenericDebugContext,
			 "xsltCopyText: copy CDATA text %s\n",
			 cur->content));
    } else if (cur->name == xmlStringTextNoenc) {
	XSLT_TRACE(ctxt,XSLT_TRACE_COPY_TEXT,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltCopyText: copy unescaped text %s\n",
			 cur->content));
    } else {
	XSLT_TRACE(ctxt,XSLT_TRACE_COPY_TEXT,xsltGenericDebug(xsltGenericDebugContext,
			 "xsltCopyText: copy text %s\n",
			 cur->content));
    }
#endif

    /*
    * Play save and reset the merging mechanism for every new
    * target node.
    */
    if ((target == NULL) || (target->children == NULL)) {
	ctxt->lasttext = NULL;
    }

    if ((ctxt->style->cdataSection != NULL) &&
	(ctxt->type == XSLT_OUTPUT_XML) &&
	(target != NULL) &&
	(target->type == XML_ELEMENT_NODE) &&
	(((target->ns == NULL) &&
	  (xmlHashLookup2(ctxt->style->cdataSection,
		          target->name, NULL) != NULL)) ||
	 ((target->ns != NULL) &&
	  (xmlHashLookup2(ctxt->style->cdataSection,
	                  target->name, target->ns->href) != NULL))))
    {
	/*
	* Process "cdata-section-elements".
	*/
	/*
	* OPTIMIZE TODO: xsltCopyText() is also used for attribute content.
	*/
	/*
	* TODO: Since this doesn't merge adjacent CDATA-section nodes,
	* we'll get: <![CDATA[x]]><!CDATA[y]]>.
	* TODO: Reported in #321505.
	*/
	if ((target->last != NULL) &&
	     (target->last->type == XML_CDATA_SECTION_NODE))
	{
	    /*
	    * Append to existing CDATA-section node.
	    */
	    copy = xsltAddTextString(ctxt, target->last, cur->content,
		xmlStrlen(cur->content));
	    goto exit;
	} else {
	    unsigned int len;

	    len = xmlStrlen(cur->content);
	    copy = xmlNewCDataBlock(ctxt->output, cur->content, len);
	    if (copy == NULL)
		goto exit;
	    ctxt->lasttext = copy->content;
	    ctxt->lasttsize = len;
	    ctxt->lasttuse = len;
	}
    } else if ((target != NULL) &&
	(target->last != NULL) &&
	/* both escaped or both non-escaped text-nodes */
	(((target->last->type == XML_TEXT_NODE) &&
	(target->last->name == cur->name)) ||
        /* non-escaped text nodes and CDATA-section nodes */
	(((target->last->type == XML_CDATA_SECTION_NODE) &&
	(cur->name == xmlStringTextNoenc)))))
    {
	/*
	 * we are appending to an existing text node
	 */
	copy = xsltAddTextString(ctxt, target->last, cur->content,
	    xmlStrlen(cur->content));
	goto exit;
    } else if ((interned) && (target != NULL) &&
	(target->doc != NULL) &&
	(target->doc->dict == ctxt->dict))
    {
	/*
	* TODO: DO we want to use this also for "text" output?
	*/
        copy = xmlNewTextLen(NULL, 0);
	if (copy == NULL)
	    goto exit;
	if (cur->name == xmlStringTextNoenc)
	    copy->name = xmlStringTextNoenc;

	/*
	 * Must confirm that content is in dict (bug 302821)
	 * TODO: This check should be not needed for text coming
	 * from the stylesheets
	 */
	if (xmlDictOwns(ctxt->dict, cur->content))
	    copy->content = cur->content;
	else {
	    if ((copy->content = xmlStrdup(cur->content)) == NULL)
		return NULL;
	}

	ctxt->lasttext = NULL;
    } else {
        /*
	 * normal processing. keep counters to extend the text node
	 * in xsltAddTextString if needed.
	 */
        unsigned int len;

	len = xmlStrlen(cur->content);
	copy = xmlNewTextLen(cur->content, len);
	if (copy == NULL)
	    goto exit;
	if (cur->name == xmlStringTextNoenc)
	    copy->name = xmlStringTextNoenc;
	ctxt->lasttext = copy->content;
	ctxt->lasttsize = len;
	ctxt->lasttuse = len;
    }
    if (copy != NULL) {
	if (target != NULL) {
	    copy->doc = target->doc;
	    /*
	    * MAYBE TODO: Maybe we should reset the ctxt->lasttext here
	    *  to ensure that the optimized text-merging mechanism
	    *  won't interfere with normal node-merging in any case.
	    */
	    copy = xsltAddChild(target, copy);
	}
    } else {
	xsltTransformError(ctxt, NULL, target,
			 "xsltCopyText: text copy failed\n");
    }

exit:
    if ((copy == NULL) || (copy->content == NULL)) {
	xsltTransformError(ctxt, NULL, target,
	    "Internal error in xsltCopyText(): "
	    "Failed to copy the string.\n");
	ctxt->state = XSLT_STATE_STOPPED;
    }
    return(copy);
}