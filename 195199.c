xmlXIncludeCopyNode(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	            xmlDocPtr source, xmlNodePtr elem) {
    xmlNodePtr result = NULL;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(elem == NULL))
	return(NULL);
    if (elem->type == XML_DTD_NODE)
	return(NULL);
    if (elem->type == XML_DOCUMENT_NODE)
	result = xmlXIncludeCopyNodeList(ctxt, target, source, elem->children);
    else
        result = xmlDocCopyNode(elem, target, 1);
    return(result);
}