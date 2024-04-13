xsltCompileXSLTIncludeElem(xsltCompilerCtxtPtr cctxt, xmlNodePtr node) {
    xsltStyleItemIncludePtr item;

    if ((cctxt == NULL) || (node == NULL))
	return(NULL);

    node->psvi = NULL;
    item = (xsltStyleItemIncludePtr) xmlMalloc(sizeof(xsltStyleItemInclude));
    if (item == NULL) {
	xsltTransformError(NULL, cctxt->style, node,
		"xsltIncludeComp : malloc failed\n");
	cctxt->style->errors++;
	return(NULL);
    }
    memset(item, 0, sizeof(xsltStyleItemInclude));

    node->psvi = item;
    item->inst = node;
    item->type = XSLT_FUNC_INCLUDE;

    item->next = cctxt->style->preComps;
    cctxt->style->preComps = (xsltElemPreCompPtr) item;

    return(item);
}