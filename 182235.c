xsltCopy(xsltTransformContextPtr ctxt, xmlNodePtr node,
	 xmlNodePtr inst, xsltStylePreCompPtr castedComp)
{
#ifdef XSLT_REFACTORED
    xsltStyleItemCopyPtr comp = (xsltStyleItemCopyPtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
#endif
    xmlNodePtr copy, oldInsert;

    oldInsert = ctxt->insert;
    if (ctxt->insert != NULL) {
	switch (node->type) {
	    case XML_TEXT_NODE:
	    case XML_CDATA_SECTION_NODE:
		/*
		 * This text comes from the stylesheet
		 * For stylesheets, the set of whitespace-preserving
		 * element names consists of just xsl:text.
		 */
#ifdef WITH_XSLT_DEBUG_PROCESS
		if (node->type == XML_CDATA_SECTION_NODE) {
		    XSLT_TRACE(ctxt,XSLT_TRACE_COPY,xsltGenericDebug(xsltGenericDebugContext,
			 "xsltCopy: CDATA text %s\n", node->content));
		} else {
		    XSLT_TRACE(ctxt,XSLT_TRACE_COPY,xsltGenericDebug(xsltGenericDebugContext,
			 "xsltCopy: text %s\n", node->content));
                }
#endif
		xsltCopyText(ctxt, ctxt->insert, node, 0);
		break;
	    case XML_DOCUMENT_NODE:
	    case XML_HTML_DOCUMENT_NODE:
		break;
	    case XML_ELEMENT_NODE:
		/*
		* REVISIT NOTE: The "fake" is a doc-node, not an element node.
		* REMOVED:
		*   if (xmlStrEqual(node->name, BAD_CAST " fake node libxslt"))
		*    return;
		*/

#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_COPY,xsltGenericDebug(xsltGenericDebugContext,
				 "xsltCopy: node %s\n", node->name));
#endif
		copy = xsltShallowCopyElem(ctxt, node, ctxt->insert, 0);
		ctxt->insert = copy;
		if (comp->use != NULL) {
		    xsltApplyAttributeSet(ctxt, node, inst, comp->use);
		}
		break;
	    case XML_ATTRIBUTE_NODE: {
#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_COPY,xsltGenericDebug(xsltGenericDebugContext,
				 "xsltCopy: attribute %s\n", node->name));
#endif
		/*
		* REVISIT: We could also raise an error if the parent is not
		* an element node.
		* OPTIMIZE TODO: Can we set the value/children of the
		* attribute without an intermediate copy of the string value?
		*/
		xsltShallowCopyAttr(ctxt, inst, ctxt->insert, (xmlAttrPtr) node);
		break;
	    }
	    case XML_PI_NODE:
#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_COPY,xsltGenericDebug(xsltGenericDebugContext,
				 "xsltCopy: PI %s\n", node->name));
#endif
		copy = xmlNewDocPI(ctxt->insert->doc, node->name,
		                   node->content);
		copy = xsltAddChild(ctxt->insert, copy);
		break;
	    case XML_COMMENT_NODE:
#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_COPY,xsltGenericDebug(xsltGenericDebugContext,
				 "xsltCopy: comment\n"));
#endif
		copy = xmlNewComment(node->content);
		copy = xsltAddChild(ctxt->insert, copy);
		break;
	    case XML_NAMESPACE_DECL:
#ifdef WITH_XSLT_DEBUG_PROCESS
		XSLT_TRACE(ctxt,XSLT_TRACE_COPY,xsltGenericDebug(xsltGenericDebugContext,
				 "xsltCopy: namespace declaration\n"));
#endif
		xsltShallowCopyNsNode(ctxt, inst, ctxt->insert, (xmlNsPtr)node);
		break;
	    default:
		break;

	}
    }

    switch (node->type) {
	case XML_DOCUMENT_NODE:
	case XML_HTML_DOCUMENT_NODE:
	case XML_ELEMENT_NODE:
	    xsltApplySequenceConstructor(ctxt, ctxt->node, inst->children,
		NULL);
	    break;
	default:
	    break;
    }
    ctxt->insert = oldInsert;
}