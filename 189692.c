xsltCompileRelativePathPattern(xsltParserContextPtr ctxt, xmlChar *token, int novar) {
    xsltCompileStepPattern(ctxt, token, novar);
    if (ctxt->error)
	goto error;
    SKIP_BLANKS;
    while ((CUR != 0) && (CUR != '|')) {
	if ((CUR == '/') && (NXT(1) == '/')) {
	    PUSH(XSLT_OP_ANCESTOR, NULL, NULL, novar);
	    NEXT;
	    NEXT;
	    SKIP_BLANKS;
	    xsltCompileStepPattern(ctxt, NULL, novar);
	} else if (CUR == '/') {
	    PUSH(XSLT_OP_PARENT, NULL, NULL, novar);
	    NEXT;
	    SKIP_BLANKS;
	    if ((CUR != 0) && (CUR != '|')) {
		xsltCompileRelativePathPattern(ctxt, NULL, novar);
	    }
	} else {
	    ctxt->error = 1;
	}
	if (ctxt->error)
	    goto error;
	SKIP_BLANKS;
    }
error:
    return;
}