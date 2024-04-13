xsltComment(xsltTransformContextPtr ctxt, xmlNodePtr node,
	           xmlNodePtr inst, xsltStylePreCompPtr comp ATTRIBUTE_UNUSED) {
    xmlChar *value = NULL;
    xmlNodePtr commentNode;
    int len;

    value = xsltEvalTemplateString(ctxt, node, inst);
    /* TODO: use or generate the compiled form */
    len = xmlStrlen(value);
    if (len > 0) {
        if ((value[len-1] == '-') ||
	    (xmlStrstr(value, BAD_CAST "--"))) {
	    xsltTransformError(ctxt, NULL, inst,
		    "xsl:comment : '--' or ending '-' not allowed in comment\n");
	    /* fall through to try to catch further errors */
	}
    }
#ifdef WITH_XSLT_DEBUG_PROCESS
    if (value == NULL) {
	XSLT_TRACE(ctxt,XSLT_TRACE_COMMENT,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltComment: empty\n"));
    } else {
	XSLT_TRACE(ctxt,XSLT_TRACE_COMMENT,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltComment: content %s\n", value));
    }
#endif

    commentNode = xmlNewComment(value);
    commentNode = xsltAddChild(ctxt->insert, commentNode);

    if (value != NULL)
	xmlFree(value);
}