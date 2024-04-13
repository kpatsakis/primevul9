xsltCleanupStylesheetTree(xmlDocPtr doc ATTRIBUTE_UNUSED,
			  xmlNodePtr rootElem ATTRIBUTE_UNUSED)
{    
#if 0 /* TODO: Currently disabled, since probably not needed. */
    xmlNodePtr cur;

    if ((doc == NULL) || (rootElem == NULL) ||
	(rootElem->type != XML_ELEMENT_NODE) ||
	(doc != rootElem->doc))
	return(-1);

    /*
    * Cleanup was suggested by Aleksey Sanin:
    * Clear the PSVI field to avoid problems if the
    * node-tree of the stylesheet is intended to be used for
    * further processing by the user (e.g. for compiling it
    * once again - although not recommended).
    */

    cur = rootElem;
    while (cur != NULL) {
	if (cur->type == XML_ELEMENT_NODE) {
	    /*
	    * Clear the PSVI field.
	    */
	    cur->psvi = NULL;
	    if (cur->children) {
		cur = cur->children;
		continue;
	    }
	}

leave_node:
	if (cur == rootElem)
	    break;
	if (cur->next != NULL)
	    cur = cur->next;
	else {
	    cur = cur->parent;
	    if (cur == NULL)
		break;
	    goto leave_node;
	}
    }
#endif /* #if 0 */
    return(0);
}