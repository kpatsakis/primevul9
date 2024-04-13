exsltDynEvaluateFunction(xmlXPathParserContextPtr ctxt, int nargs) {
	xmlChar *str = NULL;
	xmlXPathObjectPtr ret = NULL;

	if (ctxt == NULL)
		return;
	if (nargs != 1) {
		xsltPrintErrorContext(xsltXPathGetTransformContext(ctxt), NULL, NULL);
        xsltGenericError(xsltGenericErrorContext,
			"dyn:evalute() : invalid number of args %d\n", nargs);
		ctxt->error = XPATH_INVALID_ARITY;
		return;
	}
	str = xmlXPathPopString(ctxt);
	/* return an empty node-set if an empty string is passed in */
	if (!str||!xmlStrlen(str)) {
		if (str) xmlFree(str);
		valuePush(ctxt,xmlXPathNewNodeSet(NULL));
		return;
	}
	ret = xmlXPathEval(str,ctxt->context);
	if (ret)
		valuePush(ctxt,ret);
	else {
		xsltGenericError(xsltGenericErrorContext,
			"dyn:evaluate() : unable to evaluate expression '%s'\n",str);
		valuePush(ctxt,xmlXPathNewNodeSet(NULL));
	}
	xmlFree(str);
	return;
}