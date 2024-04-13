xsltApplyImports(xsltTransformContextPtr ctxt, xmlNodePtr contextNode,
	         xmlNodePtr inst,
		 xsltStylePreCompPtr comp ATTRIBUTE_UNUSED)
{
    xsltTemplatePtr templ;

    if ((ctxt == NULL) || (inst == NULL))
	return;

    if (comp == NULL) {
	xsltTransformError(ctxt, NULL, inst,
	    "Internal error in xsltApplyImports(): "
	    "The XSLT 'apply-imports' instruction was not compiled.\n");
	return;
    }
    /*
    * NOTE that ctxt->currentTemplateRule and ctxt->templ is not the
    * same; the former is the "Current Template Rule" as defined by the
    * XSLT spec, the latter is simply the template struct being
    * currently processed.
    */
    if (ctxt->currentTemplateRule == NULL) {
	/*
	* SPEC XSLT 2.0:
	* "[ERR XTDE0560] It is a non-recoverable dynamic error if
	*  xsl:apply-imports or xsl:next-match is evaluated when the
	*  current template rule is null."
	*/
	xsltTransformError(ctxt, NULL, inst,
	     "It is an error to call 'apply-imports' "
	     "when there's no current template rule.\n");
	return;
    }
    /*
    * TODO: Check if this is correct.
    */
    templ = xsltGetTemplate(ctxt, contextNode,
	ctxt->currentTemplateRule->style);

    if (templ != NULL) {
	xsltTemplatePtr oldCurTemplRule = ctxt->currentTemplateRule;
	/*
	* Set the current template rule.
	*/
	ctxt->currentTemplateRule = templ;
	/*
	* URGENT TODO: Need xsl:with-param be handled somehow here?
	*/
	xsltApplyXSLTTemplate(ctxt, contextNode, templ->content,
	    templ, NULL);

	ctxt->currentTemplateRule = oldCurTemplRule;
    }
}