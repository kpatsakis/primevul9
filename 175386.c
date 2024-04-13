xmlXPathOrderDocElems(xmlDocPtr doc) {
    ptrdiff_t count = 0;
    xmlNodePtr cur;

    if (doc == NULL)
	return(-1);
    cur = doc->children;
    while (cur != NULL) {
	if (cur->type == XML_ELEMENT_NODE) {
	    cur->content = (void *) (-(++count));
	    if (cur->children != NULL) {
		cur = cur->children;
		continue;
	    }
	}
	if (cur->next != NULL) {
	    cur = cur->next;
	    continue;
	}
	do {
	    cur = cur->parent;
	    if (cur == NULL)
		break;
	    if (cur == (xmlNodePtr) doc) {
		cur = NULL;
		break;
	    }
	    if (cur->next != NULL) {
		cur = cur->next;
		break;
	    }
	} while (cur != NULL);
    }
    return((long) count);
}