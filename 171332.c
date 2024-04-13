xsltParseRemoveWhitespace(xmlNodePtr node)
{
    if ((node == NULL) || (node->children == NULL))
	return(0);
    else {
	xmlNodePtr delNode = NULL, child = node->children;

	do {
	    if (delNode) {
		xmlUnlinkNode(delNode);
		xmlFreeNode(delNode);
		delNode = NULL;
	    }
	    if (((child->type == XML_TEXT_NODE) ||
		 (child->type == XML_CDATA_SECTION_NODE)) &&
		(IS_BLANK_NODE(child)))
		delNode = child;	    
	    child = child->next;
	} while (child != NULL);
	if (delNode) {
	    xmlUnlinkNode(delNode);
	    xmlFreeNode(delNode);
	    delNode = NULL;
	}
    }
    return(0);
}