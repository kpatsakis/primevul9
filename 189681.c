xsltNewParserContext(xsltStylesheetPtr style, xsltTransformContextPtr ctxt) {
    xsltParserContextPtr cur;

    cur = (xsltParserContextPtr) xmlMalloc(sizeof(xsltParserContext));
    if (cur == NULL) {
	xsltTransformError(NULL, NULL, NULL,
		"xsltNewParserContext : malloc failed\n");
	return(NULL);
    }
    memset(cur, 0, sizeof(xsltParserContext));
    cur->style = style;
    cur->ctxt = ctxt;
    return(cur);
}