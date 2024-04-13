xsltComputeAllKeys(xsltTransformContextPtr ctxt, xmlNodePtr contextNode)
{
    if ((ctxt == NULL) || (contextNode == NULL)) {
	xsltTransformError(ctxt, NULL, ctxt->inst,
	    "Internal error in xsltComputeAllKeys(): "
	    "Bad arguments.\n");
	return(-1);
    }

    if (ctxt->document == NULL) {
	/*
	* The document info will only be NULL if we have a RTF.
	*/
	if (contextNode->doc->_private != NULL)
	    goto doc_info_mismatch;
	/*
	* On-demand creation of the document info (needed for keys).
	*/
	ctxt->document = xsltNewDocument(ctxt, contextNode->doc);
	if (ctxt->document == NULL)
	    return(-1);
    }
    return xsltInitAllDocKeys(ctxt);

doc_info_mismatch:
    xsltTransformError(ctxt, NULL, ctxt->inst,
	"Internal error in xsltComputeAllKeys(): "
	"The context's document info doesn't match the "
	"document info of the current result tree.\n");
    ctxt->state = XSLT_STATE_STOPPED;
    return(-1);
}