xsltCopyTreeList(xsltTransformContextPtr ctxt, xmlNodePtr invocNode,
		 xmlNodePtr list,
		 xmlNodePtr insert, int isLRE, int topElemVisited)
{
    xmlNodePtr copy, ret = NULL;

    while (list != NULL) {
	copy = xsltCopyTreeInternal(ctxt, invocNode,
	    list, insert, isLRE, topElemVisited);
	if (copy != NULL) {
	    if (ret == NULL) {
		ret = copy;
	    }
	}
	list = list->next;
    }
    return(ret);
}