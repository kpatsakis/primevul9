xsltText(xsltTransformContextPtr ctxt, xmlNodePtr node ATTRIBUTE_UNUSED,
	    xmlNodePtr inst, xsltStylePreCompPtr comp ATTRIBUTE_UNUSED) {
    if ((inst->children != NULL) && (comp != NULL)) {
	xmlNodePtr text = inst->children;
	xmlNodePtr copy;

	while (text != NULL) {
	    if ((text->type != XML_TEXT_NODE) &&
	         (text->type != XML_CDATA_SECTION_NODE)) {
		xsltTransformError(ctxt, NULL, inst,
				 "xsl:text content problem\n");
		break;
	    }
	    copy = xmlNewDocText(ctxt->output, text->content);
	    if (text->type != XML_CDATA_SECTION_NODE) {
#ifdef WITH_XSLT_DEBUG_PARSING
		xsltGenericDebug(xsltGenericDebugContext,
		     "Disable escaping: %s\n", text->content);
#endif
		copy->name = xmlStringTextNoenc;
	    }
	    copy = xsltAddChild(ctxt->insert, copy);
	    text = text->next;
	}
    }
}