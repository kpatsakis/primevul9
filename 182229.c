xsltCallTemplate(xsltTransformContextPtr ctxt, xmlNodePtr node,
	           xmlNodePtr inst, xsltStylePreCompPtr castedComp)
{
#ifdef XSLT_REFACTORED
    xsltStyleItemCallTemplatePtr comp =
	(xsltStyleItemCallTemplatePtr) castedComp;
#else
    xsltStylePreCompPtr comp = castedComp;
#endif
    xsltStackElemPtr withParams = NULL;

    if (ctxt->insert == NULL)
	return;
    if (comp == NULL) {
	xsltTransformError(ctxt, NULL, inst,
	     "The XSLT 'call-template' instruction was not compiled.\n");
	return;
    }

    /*
     * The template must have been precomputed
     */
    if (comp->templ == NULL) {
	comp->templ = xsltFindTemplate(ctxt, comp->name, comp->ns);
	if (comp->templ == NULL) {
	    if (comp->ns != NULL) {
	        xsltTransformError(ctxt, NULL, inst,
			"The called template '{%s}%s' was not found.\n",
			comp->ns, comp->name);
	    } else {
	        xsltTransformError(ctxt, NULL, inst,
			"The called template '%s' was not found.\n",
			comp->name);
	    }
	    return;
	}
    }

#ifdef WITH_XSLT_DEBUG_PROCESS
    if ((comp != NULL) && (comp->name != NULL))
	XSLT_TRACE(ctxt,XSLT_TRACE_CALL_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
			 "call-template: name %s\n", comp->name));
#endif

    if (inst->children) {
	xmlNodePtr cur;
	xsltStackElemPtr param;

	cur = inst->children;
	while (cur != NULL) {
#ifdef WITH_DEBUGGER
	    if (ctxt->debugStatus != XSLT_DEBUG_NONE)
		xslHandleDebugger(cur, node, comp->templ, ctxt);
#endif
	    if (ctxt->state == XSLT_STATE_STOPPED) break;
	    /*
	    * TODO: The "with-param"s could be part of the "call-template"
	    *   structure. Avoid to "search" for params dynamically
	    *   in the XML tree every time.
	    */
	    if (IS_XSLT_ELEM(cur)) {
		if (IS_XSLT_NAME(cur, "with-param")) {
		    param = xsltParseStylesheetCallerParam(ctxt, cur);
		    if (param != NULL) {
			param->next = withParams;
			withParams = param;
		    }
		} else {
		    xsltGenericError(xsltGenericErrorContext,
			"xsl:call-template: misplaced xsl:%s\n", cur->name);
		}
	    } else {
		xsltGenericError(xsltGenericErrorContext,
		    "xsl:call-template: misplaced %s element\n", cur->name);
	    }
	    cur = cur->next;
	}
    }
    /*
     * Create a new frame using the params first
     */
    xsltApplyXSLTTemplate(ctxt, node, comp->templ->content, comp->templ,
	withParams);
    if (withParams != NULL)
	xsltFreeStackElemList(withParams);

#ifdef WITH_XSLT_DEBUG_PROCESS
    if ((comp != NULL) && (comp->name != NULL))
	XSLT_TRACE(ctxt,XSLT_TRACE_CALL_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
			 "call-template returned: name %s\n", comp->name));
#endif
}