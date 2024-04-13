xmlXIncludeProcessTreeFlagsData(xmlNodePtr tree, int flags, void *data) {
    xmlXIncludeCtxtPtr ctxt;
    int ret = 0;

    if ((tree == NULL) || (tree->type == XML_NAMESPACE_DECL) ||
        (tree->doc == NULL))
        return(-1);

    ctxt = xmlXIncludeNewContext(tree->doc);
    if (ctxt == NULL)
        return(-1);
    ctxt->_private = data;
    ctxt->base = xmlStrdup((xmlChar *)tree->doc->URL);
    xmlXIncludeSetFlags(ctxt, flags);
    ret = xmlXIncludeDoProcess(ctxt, tree->doc, tree, 0);
    if ((ret >= 0) && (ctxt->nbErrors > 0))
        ret = -1;

    xmlXIncludeFreeContext(ctxt);
    return(ret);
}