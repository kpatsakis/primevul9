xmlXIncludeGetNthChild(xmlNodePtr cur, int no) {
    int i;
    if ((cur == NULL) || (cur->type == XML_NAMESPACE_DECL))
        return(NULL);
    cur = cur->children;
    for (i = 0;i <= no;cur = cur->next) {
	if (cur == NULL)
	    return(cur);
	if ((cur->type == XML_ELEMENT_NODE) ||
	    (cur->type == XML_DOCUMENT_NODE) ||
	    (cur->type == XML_HTML_DOCUMENT_NODE)) {
	    i++;
	    if (i == no)
		break;
	}
    }
    return(cur);
}