xsltAllocateExtraCtxt(xsltTransformContextPtr ctxt)
{
    if (ctxt->extrasNr >= ctxt->extrasMax) {
	int i;
	if (ctxt->extrasNr == 0) {
	    ctxt->extrasMax = 20;
	    ctxt->extras = (xsltRuntimeExtraPtr)
		xmlMalloc(ctxt->extrasMax * sizeof(xsltRuntimeExtra));
	    if (ctxt->extras == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xsltAllocateExtraCtxt: out of memory\n");
		ctxt->state = XSLT_STATE_ERROR;
		return(0);
	    }
	    for (i = 0;i < ctxt->extrasMax;i++) {
		ctxt->extras[i].info = NULL;
		ctxt->extras[i].deallocate = NULL;
		ctxt->extras[i].val.ptr = NULL;
	    }

	} else {
	    xsltRuntimeExtraPtr tmp;

	    ctxt->extrasMax += 100;
	    tmp = (xsltRuntimeExtraPtr) xmlRealloc(ctxt->extras,
		            ctxt->extrasMax * sizeof(xsltRuntimeExtra));
	    if (tmp == NULL) {
		xmlGenericError(xmlGenericErrorContext,
			"xsltAllocateExtraCtxt: out of memory\n");
		ctxt->state = XSLT_STATE_ERROR;
		return(0);
	    }
	    ctxt->extras = tmp;
	    for (i = ctxt->extrasNr;i < ctxt->extrasMax;i++) {
		ctxt->extras[i].info = NULL;
		ctxt->extras[i].deallocate = NULL;
		ctxt->extras[i].val.ptr = NULL;
	    }
	}
    }
    return(ctxt->extrasNr++);
}