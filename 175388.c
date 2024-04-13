 */
static unsigned int
xmlXPathNodeValHash(xmlNodePtr node) {
    int len = 2;
    const xmlChar * string = NULL;
    xmlNodePtr tmp = NULL;
    unsigned int ret = 0;

    if (node == NULL)
	return(0);

    if (node->type == XML_DOCUMENT_NODE) {
	tmp = xmlDocGetRootElement((xmlDocPtr) node);
	if (tmp == NULL)
	    node = node->children;
	else
	    node = tmp;

	if (node == NULL)
	    return(0);
    }

    switch (node->type) {
	case XML_COMMENT_NODE:
	case XML_PI_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_TEXT_NODE:
	    string = node->content;
	    if (string == NULL)
		return(0);
	    if (string[0] == 0)
		return(0);
	    return(((unsigned int) string[0]) +
		   (((unsigned int) string[1]) << 8));
	case XML_NAMESPACE_DECL:
	    string = ((xmlNsPtr)node)->href;
	    if (string == NULL)
		return(0);
	    if (string[0] == 0)
		return(0);
	    return(((unsigned int) string[0]) +
		   (((unsigned int) string[1]) << 8));
	case XML_ATTRIBUTE_NODE:
	    tmp = ((xmlAttrPtr) node)->children;
	    break;
	case XML_ELEMENT_NODE:
	    tmp = node->children;
	    break;
	default:
	    return(0);
    }
    while (tmp != NULL) {
	switch (tmp->type) {
	    case XML_CDATA_SECTION_NODE:
	    case XML_TEXT_NODE:
		string = tmp->content;
		break;
	    default:
                string = NULL;
		break;
	}
	if ((string != NULL) && (string[0] != 0)) {
	    if (len == 1) {
		return(ret + (((unsigned int) string[0]) << 8));
	    }
	    if (string[1] == 0) {
		len = 1;
		ret = (unsigned int) string[0];
	    } else {
		return(((unsigned int) string[0]) +
		       (((unsigned int) string[1]) << 8));
	    }
	}
	/*
	 * Skip to next node
	 */
	if ((tmp->children != NULL) && (tmp->type != XML_DTD_NODE)) {
	    if (tmp->children->type != XML_ENTITY_DECL) {
		tmp = tmp->children;
		continue;
	    }
	}
	if (tmp == node)
	    break;

	if (tmp->next != NULL) {
	    tmp = tmp->next;
	    continue;
	}

	do {
	    tmp = tmp->parent;
	    if (tmp == NULL)
		break;
	    if (tmp == node) {
		tmp = NULL;
		break;
	    }
	    if (tmp->next != NULL) {
		tmp = tmp->next;
		break;
	    }
	} while (tmp != NULL);
    }