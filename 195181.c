xmlXIncludeProcessTreeFlags(xmlNodePtr tree, int flags) {
    xmlXIncludeCtxtPtr ctxt;
    int ret = 0;

    if ((tree == NULL) || (tree->type == XML_NAMESPACE_DECL) ||
        (tree->doc == NULL))
	return(-1);
    ctxt = xmlXIncludeNewContext(tree->doc);
    if (ctxt == NULL)
	return(-1);
    ctxt->base = xmlNodeGetBase(tree->doc, tree);
    xmlXIncludeSetFlags(ctxt, flags);
    ret = xmlXIncludeDoProcess(ctxt, tree->doc, tree, 0);
    if ((ret >= 0) && (ctxt->nbErrors > 0))
	ret = -1;

    xmlXIncludeFreeContext(ctxt);
    return(ret);
}