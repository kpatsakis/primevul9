xmlXIncludeIncludeNode(xmlXIncludeCtxtPtr ctxt, int nr) {
    xmlNodePtr cur, end, list, tmp;

    if (ctxt == NULL)
	return(-1);
    if ((nr < 0) || (nr >= ctxt->incNr))
	return(-1);
    cur = ctxt->incTab[nr]->ref;
    if ((cur == NULL) || (cur->type == XML_NAMESPACE_DECL))
	return(-1);

    list = ctxt->incTab[nr]->inc;
    ctxt->incTab[nr]->inc = NULL;
    ctxt->incTab[nr]->emptyFb = 0;

    /*
     * Check against the risk of generating a multi-rooted document
     */
    if ((cur->parent != NULL) &&
	(cur->parent->type != XML_ELEMENT_NODE)) {
	int nb_elem = 0;

	tmp = list;
	while (tmp != NULL) {
	    if (tmp->type == XML_ELEMENT_NODE)
		nb_elem++;
	    tmp = tmp->next;
	}
	if (nb_elem > 1) {
	    xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	                   XML_XINCLUDE_MULTIPLE_ROOT,
		       "XInclude error: would result in multiple root nodes\n",
			   NULL);
            xmlFreeNodeList(list);
	    return(-1);
	}
    }

    if (ctxt->parseFlags & XML_PARSE_NOXINCNODE) {
	/*
	 * Add the list of nodes
	 */
	while (list != NULL) {
	    end = list;
	    list = list->next;

	    xmlAddPrevSibling(cur, end);
	}
	xmlUnlinkNode(cur);
	xmlFreeNode(cur);
    } else {
        xmlNodePtr child, next;

	/*
	 * Change the current node as an XInclude start one, and add an
	 * XInclude end one
	 */
        if (ctxt->incTab[nr]->fallback)
            xmlUnsetProp(cur, BAD_CAST "href");
	cur->type = XML_XINCLUDE_START;
        /* Remove fallback children */
        for (child = cur->children; child != NULL; child = next) {
            next = child->next;
            xmlUnlinkNode(child);
            xmlFreeNode(child);
        }
	end = xmlNewDocNode(cur->doc, cur->ns, cur->name, NULL);
	if (end == NULL) {
	    xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	                   XML_XINCLUDE_BUILD_FAILED,
			   "failed to build node\n", NULL);
            xmlFreeNodeList(list);
	    return(-1);
	}
	end->type = XML_XINCLUDE_END;
	xmlAddNextSibling(cur, end);

	/*
	 * Add the list of nodes
	 */
	while (list != NULL) {
	    cur = list;
	    list = list->next;

	    xmlAddPrevSibling(end, cur);
	}
    }


    return(0);
}