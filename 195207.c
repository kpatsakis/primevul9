xmlXIncludeTestNode(xmlXIncludeCtxtPtr ctxt, xmlNodePtr node) {
    if (node == NULL)
	return(0);
    if (node->type != XML_ELEMENT_NODE)
	return(0);
    if (node->ns == NULL)
	return(0);
    if ((xmlStrEqual(node->ns->href, XINCLUDE_NS)) ||
        (xmlStrEqual(node->ns->href, XINCLUDE_OLD_NS))) {
	if (xmlStrEqual(node->ns->href, XINCLUDE_OLD_NS)) {
	    if (ctxt->legacy == 0) {
#if 0 /* wait for the XML Core Working Group to get something stable ! */
		xmlXIncludeWarn(ctxt, node, XML_XINCLUDE_DEPRECATED_NS,
	               "Deprecated XInclude namespace found, use %s",
		                XINCLUDE_NS);
#endif
	        ctxt->legacy = 1;
	    }
	}
	if (xmlStrEqual(node->name, XINCLUDE_NODE)) {
	    xmlNodePtr child = node->children;
	    int nb_fallback = 0;

	    while (child != NULL) {
		if ((child->type == XML_ELEMENT_NODE) &&
		    (child->ns != NULL) &&
		    ((xmlStrEqual(child->ns->href, XINCLUDE_NS)) ||
		     (xmlStrEqual(child->ns->href, XINCLUDE_OLD_NS)))) {
		    if (xmlStrEqual(child->name, XINCLUDE_NODE)) {
			xmlXIncludeErr(ctxt, node,
			               XML_XINCLUDE_INCLUDE_IN_INCLUDE,
				       "%s has an 'include' child\n",
				       XINCLUDE_NODE);
			return(0);
		    }
		    if (xmlStrEqual(child->name, XINCLUDE_FALLBACK)) {
			nb_fallback++;
		    }
		}
		child = child->next;
	    }
	    if (nb_fallback > 1) {
		xmlXIncludeErr(ctxt, node, XML_XINCLUDE_FALLBACKS_IN_INCLUDE,
			       "%s has multiple fallback children\n",
		               XINCLUDE_NODE);
		return(0);
	    }
	    return(1);
	}
	if (xmlStrEqual(node->name, XINCLUDE_FALLBACK)) {
	    if ((node->parent == NULL) ||
		(node->parent->type != XML_ELEMENT_NODE) ||
		(node->parent->ns == NULL) ||
		((!xmlStrEqual(node->parent->ns->href, XINCLUDE_NS)) &&
		 (!xmlStrEqual(node->parent->ns->href, XINCLUDE_OLD_NS))) ||
		(!xmlStrEqual(node->parent->name, XINCLUDE_NODE))) {
		xmlXIncludeErr(ctxt, node,
		               XML_XINCLUDE_FALLBACK_NOT_IN_INCLUDE,
			       "%s is not the child of an 'include'\n",
			       XINCLUDE_FALLBACK);
	    }
	}
    }
    return(0);
}