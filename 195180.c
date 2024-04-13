xmlXIncludeCopyRange(xmlXIncludeCtxtPtr ctxt, xmlDocPtr target,
	                xmlDocPtr source, xmlXPathObjectPtr range) {
    /* pointers to generated nodes */
    xmlNodePtr list = NULL, last = NULL, listParent = NULL;
    xmlNodePtr tmp, tmp2;
    /* pointers to traversal nodes */
    xmlNodePtr start, cur, end;
    int index1, index2;
    int level = 0, lastLevel = 0, endLevel = 0, endFlag = 0;

    if ((ctxt == NULL) || (target == NULL) || (source == NULL) ||
	(range == NULL))
	return(NULL);
    if (range->type != XPATH_RANGE)
	return(NULL);
    start = (xmlNodePtr) range->user;

    if ((start == NULL) || (start->type == XML_NAMESPACE_DECL))
	return(NULL);
    end = range->user2;
    if (end == NULL)
	return(xmlDocCopyNode(start, target, 1));
    if (end->type == XML_NAMESPACE_DECL)
        return(NULL);

    cur = start;
    index1 = range->index;
    index2 = range->index2;
    /*
     * level is depth of the current node under consideration
     * list is the pointer to the root of the output tree
     * listParent is a pointer to the parent of output tree (within
       the included file) in case we need to add another level
     * last is a pointer to the last node added to the output tree
     * lastLevel is the depth of last (relative to the root)
     */
    while (cur != NULL) {
	/*
	 * Check if our output tree needs a parent
	 */
	if (level < 0) {
	    while (level < 0) {
	        /* copy must include namespaces and properties */
	        tmp2 = xmlDocCopyNode(listParent, target, 2);
	        xmlAddChild(tmp2, list);
	        list = tmp2;
	        listParent = listParent->parent;
	        level++;
	    }
	    last = list;
	    lastLevel = 0;
	}
	/*
	 * Check whether we need to change our insertion point
	 */
	while (level < lastLevel) {
	    last = last->parent;
	    lastLevel --;
	}
	if (cur == end) {	/* Are we at the end of the range? */
	    if (cur->type == XML_TEXT_NODE) {
		const xmlChar *content = cur->content;
		int len;

		if (content == NULL) {
		    tmp = xmlNewTextLen(NULL, 0);
		} else {
		    len = index2;
		    if ((cur == start) && (index1 > 1)) {
			content += (index1 - 1);
			len -= (index1 - 1);
		    } else {
			len = index2;
		    }
		    tmp = xmlNewTextLen(content, len);
		}
		/* single sub text node selection */
		if (list == NULL)
		    return(tmp);
		/* prune and return full set */
		if (level == lastLevel)
		    xmlAddNextSibling(last, tmp);
		else
		    xmlAddChild(last, tmp);
		return(list);
	    } else {	/* ending node not a text node */
	        endLevel = level;	/* remember the level of the end node */
		endFlag = 1;
		/* last node - need to take care of properties + namespaces */
		tmp = xmlDocCopyNode(cur, target, 2);
		if (list == NULL) {
		    list = tmp;
		    listParent = cur->parent;
		} else {
		    if (level == lastLevel)
			xmlAddNextSibling(last, tmp);
		    else {
			xmlAddChild(last, tmp);
			lastLevel = level;
		    }
		}
		last = tmp;

		if (index2 > 1) {
		    end = xmlXIncludeGetNthChild(cur, index2 - 1);
		    index2 = 0;
		}
		if ((cur == start) && (index1 > 1)) {
		    cur = xmlXIncludeGetNthChild(cur, index1 - 1);
		    index1 = 0;
		}  else {
		    cur = cur->children;
		}
		level++;	/* increment level to show change */
		/*
		 * Now gather the remaining nodes from cur to end
		 */
		continue;	/* while */
	    }
	} else if (cur == start) {	/* Not at the end, are we at start? */
	    if ((cur->type == XML_TEXT_NODE) ||
		(cur->type == XML_CDATA_SECTION_NODE)) {
		const xmlChar *content = cur->content;

		if (content == NULL) {
		    tmp = xmlNewTextLen(NULL, 0);
		} else {
		    if (index1 > 1) {
			content += (index1 - 1);
			index1 = 0;
		    }
		    tmp = xmlNewText(content);
		}
		last = list = tmp;
		listParent = cur->parent;
	    } else {		/* Not text node */
	        /*
		 * start of the range - need to take care of
		 * properties and namespaces
		 */
		tmp = xmlDocCopyNode(cur, target, 2);
		list = last = tmp;
		listParent = cur->parent;
		if (index1 > 1) {	/* Do we need to position? */
		    cur = xmlXIncludeGetNthChild(cur, index1 - 1);
		    level = lastLevel = 1;
		    index1 = 0;
		    /*
		     * Now gather the remaining nodes from cur to end
		     */
		    continue; /* while */
		}
	    }
	} else {
	    tmp = NULL;
	    switch (cur->type) {
		case XML_DTD_NODE:
		case XML_ELEMENT_DECL:
		case XML_ATTRIBUTE_DECL:
		case XML_ENTITY_NODE:
		    /* Do not copy DTD information */
		    break;
		case XML_ENTITY_DECL:
		    /* handle crossing entities -> stack needed */
		    break;
		case XML_XINCLUDE_START:
		case XML_XINCLUDE_END:
		    /* don't consider it part of the tree content */
		    break;
		case XML_ATTRIBUTE_NODE:
		    /* Humm, should not happen ! */
		    break;
		default:
		    /*
		     * Middle of the range - need to take care of
		     * properties and namespaces
		     */
		    tmp = xmlDocCopyNode(cur, target, 2);
		    break;
	    }
	    if (tmp != NULL) {
		if (level == lastLevel)
		    xmlAddNextSibling(last, tmp);
		else {
		    xmlAddChild(last, tmp);
		    lastLevel = level;
		}
		last = tmp;
	    }
	}
	/*
	 * Skip to next node in document order
	 */
	cur = xmlXPtrAdvanceNode(cur, &level);
	if (endFlag && (level >= endLevel))
	    break;
    }
    return(list);
}