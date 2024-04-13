xsltGetTemplate(xsltTransformContextPtr ctxt, xmlNodePtr node,
	        xsltStylesheetPtr style)
{
    xsltStylesheetPtr curstyle;
    xsltTemplatePtr ret = NULL;
    const xmlChar *name = NULL;
    xsltCompMatchPtr list = NULL;
    float priority;
    int keyed = 0;

    if ((ctxt == NULL) || (node == NULL))
	return(NULL);

    if (style == NULL) {
	curstyle = ctxt->style;
    } else {
	curstyle = xsltNextImport(style);
    }

    while ((curstyle != NULL) && (curstyle != style)) {
	priority = XSLT_PAT_NO_PRIORITY;
	/* TODO : handle IDs/keys here ! */
	if (curstyle->templatesHash != NULL) {
	    /*
	     * Use the top name as selector
	     */
	    switch (node->type) {
		case XML_ELEMENT_NODE:
		    if (node->name[0] == ' ')
			break;
		case XML_ATTRIBUTE_NODE:
		case XML_PI_NODE:
		    name = node->name;
		    break;
		case XML_DOCUMENT_NODE:
		case XML_HTML_DOCUMENT_NODE:
		case XML_TEXT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_COMMENT_NODE:
		case XML_ENTITY_REF_NODE:
		case XML_ENTITY_NODE:
		case XML_DOCUMENT_TYPE_NODE:
		case XML_DOCUMENT_FRAG_NODE:
		case XML_NOTATION_NODE:
		case XML_DTD_NODE:
		case XML_ELEMENT_DECL:
		case XML_ATTRIBUTE_DECL:
		case XML_ENTITY_DECL:
		case XML_NAMESPACE_DECL:
		case XML_XINCLUDE_START:
		case XML_XINCLUDE_END:
		    break;
		default:
		    return(NULL);

	    }
	}
	if (name != NULL) {
	    /*
	     * find the list of applicable expressions based on the name
	     */
	    list = (xsltCompMatchPtr) xmlHashLookup3(curstyle->templatesHash,
					     name, ctxt->mode, ctxt->modeURI);
	} else
	    list = NULL;
	while (list != NULL) {
	    if (xsltTestCompMatch(ctxt, list, node,
			          ctxt->mode, ctxt->modeURI)) {
		ret = list->template;
		priority = list->priority;
		break;
	    }
	    list = list->next;
	}
	list = NULL;

	/*
	 * find alternate generic matches
	 */
	switch (node->type) {
	    case XML_ELEMENT_NODE:
		if (node->name[0] == ' ')
		    list = curstyle->rootMatch;
		else
		    list = curstyle->elemMatch;
		if (node->psvi != NULL) keyed = 1;
		break;
	    case XML_ATTRIBUTE_NODE: {
	        xmlAttrPtr attr;

		list = curstyle->attrMatch;
		attr = (xmlAttrPtr) node;
		if (attr->psvi != NULL) keyed = 1;
		break;
	    }
	    case XML_PI_NODE:
		list = curstyle->piMatch;
		if (node->psvi != NULL) keyed = 1;
		break;
	    case XML_DOCUMENT_NODE:
	    case XML_HTML_DOCUMENT_NODE: {
	        xmlDocPtr doc;

		list = curstyle->rootMatch;
		doc = (xmlDocPtr) node;
		if (doc->psvi != NULL) keyed = 1;
		break;
	    }
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
		list = curstyle->textMatch;
		if (node->psvi != NULL) keyed = 1;
		break;
	    case XML_COMMENT_NODE:
		list = curstyle->commentMatch;
		if (node->psvi != NULL) keyed = 1;
		break;
	    case XML_ENTITY_REF_NODE:
	    case XML_ENTITY_NODE:
	    case XML_DOCUMENT_TYPE_NODE:
	    case XML_DOCUMENT_FRAG_NODE:
	    case XML_NOTATION_NODE:
	    case XML_DTD_NODE:
	    case XML_ELEMENT_DECL:
	    case XML_ATTRIBUTE_DECL:
	    case XML_ENTITY_DECL:
	    case XML_NAMESPACE_DECL:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
		break;
	    default:
		break;
	}
	while ((list != NULL) &&
	       ((ret == NULL)  || (list->priority > priority))) {
	    if (xsltTestCompMatch(ctxt, list, node,
			          ctxt->mode, ctxt->modeURI)) {
		ret = list->template;
		priority = list->priority;
		break;
	    }
	    list = list->next;
	}
	/*
	 * Some of the tests for elements can also apply to documents
	 */
	if ((node->type == XML_DOCUMENT_NODE) ||
	    (node->type == XML_HTML_DOCUMENT_NODE) ||
	    (node->type == XML_TEXT_NODE)) {
	    list = curstyle->elemMatch;
	    while ((list != NULL) &&
		   ((ret == NULL)  || (list->priority > priority))) {
		if (xsltTestCompMatch(ctxt, list, node,
				      ctxt->mode, ctxt->modeURI)) {
		    ret = list->template;
		    priority = list->priority;
		    break;
		}
		list = list->next;
	    }
	} else if ((node->type == XML_PI_NODE) ||
		   (node->type == XML_COMMENT_NODE)) {
	    list = curstyle->elemMatch;
	    while ((list != NULL) &&
		   ((ret == NULL)  || (list->priority > priority))) {
		if (xsltTestCompMatch(ctxt, list, node,
				      ctxt->mode, ctxt->modeURI)) {
		    ret = list->template;
		    priority = list->priority;
		    break;
		}
		list = list->next;
	    }
	}

keyed_match:
	if (keyed) {
	    list = curstyle->keyMatch;
	    while ((list != NULL) &&
		   ((ret == NULL)  || (list->priority > priority))) {
		if (xsltTestCompMatch(ctxt, list, node,
				      ctxt->mode, ctxt->modeURI)) {
		    ret = list->template;
		    priority = list->priority;
		    break;
		}
		list = list->next;
	    }
	}
	else if (ctxt->hasTemplKeyPatterns &&
	    ((ctxt->document == NULL) ||
	     (ctxt->document->nbKeysComputed < ctxt->nbKeys)))
	{
	    /*
	    * Compute all remaining keys for this document.
	    *
	    * REVISIT TODO: I think this could be further optimized.
	    */
	    if (xsltComputeAllKeys(ctxt, node) == -1)
		goto error;

	    switch (node->type) {
		case XML_ELEMENT_NODE:		    
		    if (node->psvi != NULL) keyed = 1;
		    break;
		case XML_ATTRIBUTE_NODE:
		    if (((xmlAttrPtr) node)->psvi != NULL) keyed = 1;
		    break;
		case XML_TEXT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_COMMENT_NODE:
		case XML_PI_NODE:		
		    if (node->psvi != NULL) keyed = 1;
		    break;
		case XML_DOCUMENT_NODE:
		case XML_HTML_DOCUMENT_NODE:
		    if (((xmlDocPtr) node)->psvi != NULL) keyed = 1;
		    break;		
		default:
		    break;
	    }
	    if (keyed)
		goto keyed_match;
	}
	if (ret != NULL)
	    return(ret);

	/*
	 * Cycle on next curstylesheet import.
	 */
	curstyle = xsltNextImport(curstyle);
    }

error:
    return(NULL);
}