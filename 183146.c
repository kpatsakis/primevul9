xmlGetLineNoInternal(const xmlNode *node, int depth)
{
    long result = -1;

    if (depth >= 5)
        return(-1);

    if (!node)
        return result;
    if ((node->type == XML_ELEMENT_NODE) ||
        (node->type == XML_TEXT_NODE) ||
	(node->type == XML_COMMENT_NODE) ||
	(node->type == XML_PI_NODE)) {
	if (node->line == 65535) {
	    if ((node->type == XML_TEXT_NODE) && (node->psvi != NULL))
	        result = (long) node->psvi;
	    else if ((node->type == XML_ELEMENT_NODE) &&
	             (node->children != NULL))
	        result = xmlGetLineNoInternal(node->children, depth + 1);
	    else if (node->next != NULL)
	        result = xmlGetLineNoInternal(node->next, depth + 1);
	    else if (node->prev != NULL)
	        result = xmlGetLineNoInternal(node->prev, depth + 1);
	}
	if ((result == -1) || (result == 65535))
	    result = (long) node->line;
    } else if ((node->prev != NULL) &&
             ((node->prev->type == XML_ELEMENT_NODE) ||
	      (node->prev->type == XML_TEXT_NODE) ||
	      (node->prev->type == XML_COMMENT_NODE) ||
	      (node->prev->type == XML_PI_NODE)))
        result = xmlGetLineNoInternal(node->prev, depth + 1);
    else if ((node->parent != NULL) &&
             (node->parent->type == XML_ELEMENT_NODE))
        result = xmlGetLineNoInternal(node->parent, depth + 1);

    return result;
}