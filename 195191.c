xmlXIncludeCopyNodeList(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	                xmlDocPtr source, xmlNodePtr elem) {
    xmlNodePtr cur, res, result = NULL, last = NULL;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(elem == NULL))
	return(NULL);
    cur = elem;
    while (cur != NULL) {
	res = xmlXIncludeCopyNode(ctxt, target, source, cur);
	if (res != NULL) {
	    if (result == NULL) {
		result = last = res;
	    } else {
		last->next = res;
		res->prev = last;
		last = res;
	    }
	}
	cur = cur->next;
    }
    return(result);
}