xsltApplyXSLTTemplate(xsltTransformContextPtr ctxt,
		      xmlNodePtr contextNode,
		      xmlNodePtr list,
		      xsltTemplatePtr templ,
		      xsltStackElemPtr withParams)
{
    int oldVarsBase = 0;
    xmlNodePtr cur;
    xsltStackElemPtr tmpParam = NULL;
    xmlDocPtr oldUserFragmentTop;
#ifdef WITH_PROFILER
    long start = 0;
#endif

#ifdef XSLT_REFACTORED
    xsltStyleItemParamPtr iparam;
#else
    xsltStylePreCompPtr iparam;
#endif

#ifdef WITH_DEBUGGER
    int addCallResult = 0;
#endif

    if (ctxt == NULL)
	return;
    if (templ == NULL) {
	xsltTransformError(ctxt, NULL, list,
	    "xsltApplyXSLTTemplate: Bad arguments; @templ is mandatory.\n");
	return;
    }

#ifdef WITH_DEBUGGER
    if (ctxt->debugStatus != XSLT_DEBUG_NONE) {
	if (xsltDebuggerStartSequenceConstructor(ctxt, contextNode,
		list, templ, &addCallResult) == NULL)
	    return;
    }
#endif

    if (list == NULL)
        return;
    CHECK_STOPPED;

    if (ctxt->varsNr >= ctxt->maxTemplateVars)
	{
        xsltTransformError(ctxt, NULL, list,
	    "xsltApplyXSLTTemplate: A potential infinite template recursion "
	    "was detected.\n"
	    "You can adjust maxTemplateVars (--maxvars) in order to "
	    "raise the maximum number of variables/params (currently set to %d).\n",
	    ctxt->maxTemplateVars);
        xsltDebug(ctxt, contextNode, list, NULL);
	ctxt->state = XSLT_STATE_STOPPED;
        return;
	}

    oldUserFragmentTop = ctxt->tmpRVT;
    ctxt->tmpRVT = NULL;

    /*
    * Initiate a distinct scope of local params/variables.
    */
    oldVarsBase = ctxt->varsBase;
    ctxt->varsBase = ctxt->varsNr;

    ctxt->node = contextNode;

#ifdef WITH_PROFILER
    if (ctxt->profile) {
	templ->nbCalls++;
	start = xsltTimestamp();
	profPush(ctxt, 0);
	profCallgraphAdd(templ, ctxt->templ);
    }
#endif

    /*
    * Push the xsl:template declaration onto the stack.
    */
    templPush(ctxt, templ);

#ifdef WITH_XSLT_DEBUG_PROCESS
    if (templ->name != NULL)
	XSLT_TRACE(ctxt,XSLT_TRACE_APPLY_TEMPLATE,xsltGenericDebug(xsltGenericDebugContext,
	"applying xsl:template '%s'\n", templ->name));
#endif
    /*
    * Process xsl:param instructions and skip those elements for
    * further processing.
    */
    cur = list;
    do {
	if (cur->type == XML_TEXT_NODE) {
	    cur = cur->next;
	    continue;
	}
	if ((cur->type != XML_ELEMENT_NODE) ||
	    (cur->name[0] != 'p') ||
	    (cur->psvi == NULL) ||
	    (! xmlStrEqual(cur->name, BAD_CAST "param")) ||
	    (! IS_XSLT_ELEM(cur)))
	{
	    break;
	}

	list = cur->next;

#ifdef XSLT_REFACTORED
	iparam = (xsltStyleItemParamPtr) cur->psvi;
#else
	iparam = (xsltStylePreCompPtr) cur->psvi;
#endif

	/*
	* Substitute xsl:param for a given xsl:with-param.
	* Since the XPath expression will reference the params/vars
	* by index, we need to slot the xsl:with-params in the
	* order of encountered xsl:params to keep the sequence of
	* params/variables in the stack exactly as it was at
	* compile time,
	*/
	tmpParam = NULL;
	if (withParams) {
	    tmpParam = withParams;
	    do {
		if ((tmpParam->name == (iparam->name)) &&
		    (tmpParam->nameURI == (iparam->ns)))
		{
		    /*
		    * Push the caller-parameter.
		    */
		    xsltLocalVariablePush(ctxt, tmpParam, -1);
		    break;
		}
		tmpParam = tmpParam->next;
	    } while (tmpParam != NULL);
	}
	/*
	* Push the xsl:param.
	*/
	if (tmpParam == NULL) {
	    /*
	    * Note that we must assume that the added parameter
	    * has a @depth of 0.
	    */
	    xsltParseStylesheetParam(ctxt, cur);
	}
	cur = cur->next;
    } while (cur != NULL);
    /*
    * Process the sequence constructor.
    */
    xsltApplySequenceConstructor(ctxt, contextNode, list, templ);

    /*
    * Remove remaining xsl:param and xsl:with-param items from
    * the stack. Don't free xsl:with-param items.
    */
    if (ctxt->varsNr > ctxt->varsBase)
	xsltTemplateParamsCleanup(ctxt);
    ctxt->varsBase = oldVarsBase;

    /*
    * Release user-created fragments stored in the scope
    * of xsl:template. Note that this mechanism is deprecated:
    * user code should now use xsltRegisterLocalRVT() instead
    * of the obsolete xsltRegisterTmpRVT().
    */
    if (ctxt->tmpRVT) {
	xmlDocPtr curdoc = ctxt->tmpRVT, tmp;

	while (curdoc != NULL) {
	    tmp = curdoc;
	    curdoc = (xmlDocPtr) curdoc->next;
	    xsltReleaseRVT(ctxt, tmp);
	}
    }
    ctxt->tmpRVT = oldUserFragmentTop;

    /*
    * Pop the xsl:template declaration from the stack.
    */
    templPop(ctxt);

#ifdef WITH_PROFILER
    if (ctxt->profile) {
	long spent, child, total, end;

	end = xsltTimestamp();
	child = profPop(ctxt);
	total = end - start;
	spent = total - child;
	if (spent <= 0) {
	    /*
	    * Not possible unless the original calibration failed
	    * we can try to correct it on the fly.
	    */
	    xsltCalibrateAdjust(spent);
	    spent = 0;
	}

	templ->time += spent;
	if (ctxt->profNr > 0)
	    ctxt->profTab[ctxt->profNr - 1] += total;
    }
#endif

#ifdef WITH_DEBUGGER
    if ((ctxt->debugStatus != XSLT_DEBUG_NONE) && (addCallResult)) {
        xslDropCall();
    }
#endif
}