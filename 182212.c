xsltDefaultProcessOneNode(xsltTransformContextPtr ctxt, xmlNodePtr node,
			  xsltStackElemPtr params) {
    xmlNodePtr copy;
    xmlNodePtr delete = NULL, cur;
    int nbchild = 0, oldSize;
    int childno = 0, oldPos;
    xsltTemplatePtr template;

    CHECK_STOPPED;
    /*
     * Handling of leaves
     */
    switch (node->type) {
	case XML_DOCUMENT_NODE:
	case XML_HTML_DOCUMENT_NODE:
	case XML_ELEMENT_NODE:
	    break;
	case XML_CDATA_SECTION_NODE:
#ifdef WITH_XSLT_DEBUG_PROCESS
	    XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltDefaultProcessOneNode: copy CDATA %s\n",
		node->content));
#endif
	    copy = xsltCopyText(ctxt, ctxt->insert, node, 0);
	    if (copy == NULL) {
		xsltTransformError(ctxt, NULL, node,
		 "xsltDefaultProcessOneNode: cdata copy failed\n");
	    }
	    return;
	case XML_TEXT_NODE:
#ifdef WITH_XSLT_DEBUG_PROCESS
	    if (node->content == NULL) {
		XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		 "xsltDefaultProcessOneNode: copy empty text\n"));
		return;
	    } else {
		XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		 "xsltDefaultProcessOneNode: copy text %s\n",
			node->content));
            }
#endif
	    copy = xsltCopyText(ctxt, ctxt->insert, node, 0);
	    if (copy == NULL) {
		xsltTransformError(ctxt, NULL, node,
		 "xsltDefaultProcessOneNode: text copy failed\n");
	    }
	    return;
	case XML_ATTRIBUTE_NODE:
	    cur = node->children;
	    while ((cur != NULL) && (cur->type != XML_TEXT_NODE))
		cur = cur->next;
	    if (cur == NULL) {
		xsltTransformError(ctxt, NULL, node,
		 "xsltDefaultProcessOneNode: no text for attribute\n");
	    } else {
#ifdef WITH_XSLT_DEBUG_PROCESS
		if (cur->content == NULL) {
		    XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltDefaultProcessOneNode: copy empty text\n"));
		} else {
		    XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltDefaultProcessOneNode: copy text %s\n",
			cur->content));
                }
#endif
		copy = xsltCopyText(ctxt, ctxt->insert, cur, 0);
		if (copy == NULL) {
		    xsltTransformError(ctxt, NULL, node,
		     "xsltDefaultProcessOneNode: text copy failed\n");
		}
	    }
	    return;
	default:
	    return;
    }
    /*
     * Handling of Elements: first pass, cleanup and counting
     */
    cur = node->children;
    while (cur != NULL) {
	switch (cur->type) {
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
	    case XML_DOCUMENT_NODE:
	    case XML_HTML_DOCUMENT_NODE:
	    case XML_ELEMENT_NODE:
	    case XML_PI_NODE:
	    case XML_COMMENT_NODE:
		nbchild++;
		break;
            case XML_DTD_NODE:
		/* Unlink the DTD, it's still reachable using doc->intSubset */
		if (cur->next != NULL)
		    cur->next->prev = cur->prev;
		if (cur->prev != NULL)
		    cur->prev->next = cur->next;
		break;
	    default:
#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		 "xsltDefaultProcessOneNode: skipping node type %d\n",
		                 cur->type));
#endif
		delete = cur;
	}
	cur = cur->next;
	if (delete != NULL) {
#ifdef WITH_XSLT_DEBUG_PROCESS
	    XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		 "xsltDefaultProcessOneNode: removing ignorable blank node\n"));
#endif
	    xmlUnlinkNode(delete);
	    xmlFreeNode(delete);
	    delete = NULL;
	}
    }
    if (delete != NULL) {
#ifdef WITH_XSLT_DEBUG_PROCESS
	XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltDefaultProcessOneNode: removing ignorable blank node\n"));
#endif
	xmlUnlinkNode(delete);
	xmlFreeNode(delete);
	delete = NULL;
    }

    /*
     * Handling of Elements: second pass, actual processing
     */
    oldSize = ctxt->xpathCtxt->contextSize;
    oldPos = ctxt->xpathCtxt->proximityPosition;
    cur = node->children;
    while (cur != NULL) {
	childno++;
	switch (cur->type) {
	    case XML_DOCUMENT_NODE:
	    case XML_HTML_DOCUMENT_NODE:
	    case XML_ELEMENT_NODE:
		ctxt->xpathCtxt->contextSize = nbchild;
		ctxt->xpathCtxt->proximityPosition = childno;
		xsltProcessOneNode(ctxt, cur, params);
		break;
	    case XML_CDATA_SECTION_NODE:
		template = xsltGetTemplate(ctxt, cur, NULL);
		if (template) {
#ifdef WITH_XSLT_DEBUG_PROCESS
		    XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		 "xsltDefaultProcessOneNode: applying template for CDATA %s\n",
				     cur->content));
#endif
		    /*
		    * Instantiate the xsl:template.
		    */
		    xsltApplyXSLTTemplate(ctxt, cur, template->content,
			template, params);
		} else /* if (ctxt->mode == NULL) */ {
#ifdef WITH_XSLT_DEBUG_PROCESS
		    XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltDefaultProcessOneNode: copy CDATA %s\n",
				     cur->content));
#endif
		    copy = xsltCopyText(ctxt, ctxt->insert, cur, 0);
		    if (copy == NULL) {
			xsltTransformError(ctxt, NULL, cur,
			    "xsltDefaultProcessOneNode: cdata copy failed\n");
		    }
		}
		break;
	    case XML_TEXT_NODE:
		template = xsltGetTemplate(ctxt, cur, NULL);
		if (template) {
#ifdef WITH_XSLT_DEBUG_PROCESS
		    XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
	     "xsltDefaultProcessOneNode: applying template for text %s\n",
				     cur->content));
#endif
		    ctxt->xpathCtxt->contextSize = nbchild;
		    ctxt->xpathCtxt->proximityPosition = childno;
		    /*
		    * Instantiate the xsl:template.
		    */
		    xsltApplyXSLTTemplate(ctxt, cur, template->content,
			template, params);
		} else /* if (ctxt->mode == NULL) */ {
#ifdef WITH_XSLT_DEBUG_PROCESS
		    if (cur->content == NULL) {
			XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
			 "xsltDefaultProcessOneNode: copy empty text\n"));
		    } else {
			XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltDefaultProcessOneNode: copy text %s\n",
					 cur->content));
                    }
#endif
		    copy = xsltCopyText(ctxt, ctxt->insert, cur, 0);
		    if (copy == NULL) {
			xsltTransformError(ctxt, NULL, cur,
			    "xsltDefaultProcessOneNode: text copy failed\n");
		    }
		}
		break;
	    case XML_PI_NODE:
	    case XML_COMMENT_NODE:
		template = xsltGetTemplate(ctxt, cur, NULL);
		if (template) {
#ifdef WITH_XSLT_DEBUG_PROCESS
		    if (cur->type == XML_PI_NODE) {
			XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltDefaultProcessOneNode: template found for PI %s\n",
			                 cur->name));
		    } else if (cur->type == XML_COMMENT_NODE) {
			XSLT_TRACE(ctxt,XSLT_TRACE_PROCESS_NODE,xsltGenericDebug(xsltGenericDebugContext,
		     "xsltDefaultProcessOneNode: template found for comment\n"));
                    }
#endif
		    ctxt->xpathCtxt->contextSize = nbchild;
		    ctxt->xpathCtxt->proximityPosition = childno;
		    /*
		    * Instantiate the xsl:template.
		    */
		    xsltApplyXSLTTemplate(ctxt, cur, template->content,
			template, params);
		}
		break;
	    default:
		break;
	}
	cur = cur->next;
    }
    ctxt->xpathCtxt->contextSize = oldSize;
    ctxt->xpathCtxt->proximityPosition = oldPos;
}