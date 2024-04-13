xsltSort(xsltTransformContextPtr ctxt,
	xmlNodePtr node ATTRIBUTE_UNUSED, xmlNodePtr inst,
	xsltStylePreCompPtr comp) {
    if (comp == NULL) {
	xsltTransformError(ctxt, NULL, inst,
	     "xsl:sort : compilation failed\n");
	return;
    }
    xsltTransformError(ctxt, NULL, inst,
	 "xsl:sort : improper use this should not be reached\n");
}