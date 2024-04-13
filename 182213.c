xsltProcessingInstruction(xsltTransformContextPtr ctxt, xmlNodePtr node,
	           xmlNodePtr inst, xsltStylePreCompPtr castedComp) {
#ifdef XSLT_REFACTORED
    xsltStyleItemPIPtr comp = (xsltStyleItemPIPtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
#endif
    const xmlChar *name;
    xmlChar *value = NULL;
    xmlNodePtr pi;


    if (ctxt->insert == NULL)
	return;
    if (comp->has_name == 0)
	return;
    if (comp->name == NULL) {
	name = xsltEvalAttrValueTemplate(ctxt, inst,
			    (const xmlChar *)"name", NULL);
	if (name == NULL) {
	    xsltTransformError(ctxt, NULL, inst,
		 "xsl:processing-instruction : name is missing\n");
	    goto error;
	}
    } else {
	name = comp->name;
    }
    /* TODO: check that it's both an an NCName and a PITarget. */


    value = xsltEvalTemplateString(ctxt, node, inst);
    if (xmlStrstr(value, BAD_CAST "?>") != NULL) {
	xsltTransformError(ctxt, NULL, inst,
	     "xsl:processing-instruction: '?>' not allowed within PI content\n");
	goto error;
    }
#ifdef WITH_XSLT_DEBUG_PROCESS
    if (value == NULL) {
	XSLT_TRACE(ctxt,XSLT_TRACE_PI,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltProcessingInstruction: %s empty\n", name));
    } else {
	XSLT_TRACE(ctxt,XSLT_TRACE_PI,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltProcessingInstruction: %s content %s\n", name, value));
    }
#endif

    pi = xmlNewDocPI(ctxt->insert->doc, name, value);
    pi = xsltAddChild(ctxt->insert, pi);

error:
    if ((name != NULL) && (name != comp->name))
        xmlFree((xmlChar *) name);
    if (value != NULL)
	xmlFree(value);
}