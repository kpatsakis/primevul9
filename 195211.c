xmlXIncludeProcessNode(xmlXIncludeCtxtPtr ctxt, xmlNodePtr node) {
    int ret = 0;

    if ((node == NULL) || (node->type == XML_NAMESPACE_DECL) ||
        (node->doc == NULL) || (ctxt == NULL))
	return(-1);
    ret = xmlXIncludeDoProcess(ctxt, node->doc, node, 0);
    if ((ret >= 0) && (ctxt->nbErrors > 0))
	ret = -1;
    return(ret);
}