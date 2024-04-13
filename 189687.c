xsltCompileStepPattern(xsltParserContextPtr ctxt, xmlChar *token, int novar) {
    xmlChar *name = NULL;
    const xmlChar *URI = NULL;
    xmlChar *URL = NULL;
    int level;
    xsltAxis axis = 0;

    SKIP_BLANKS;
    if ((token == NULL) && (CUR == '@')) {
	NEXT;
        axis = AXIS_ATTRIBUTE;
    }
parse_node_test:
    if (token == NULL)
	token = xsltScanNCName(ctxt);
    if (token == NULL) {
	if (CUR == '*') {
	    NEXT;
	    if (axis == AXIS_ATTRIBUTE) {
                PUSH(XSLT_OP_ATTR, NULL, NULL, novar);
            }
            else {
                PUSH(XSLT_OP_ALL, NULL, NULL, novar);
            }
	    goto parse_predicate;
	} else {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileStepPattern : Name expected\n");
	    ctxt->error = 1;
	    goto error;
	}
    }


    SKIP_BLANKS;
    if (CUR == '(') {
	xsltCompileIdKeyPattern(ctxt, token, 0, novar, axis);
	if (ctxt->error)
	    goto error;
    } else if (CUR == ':') {
	NEXT;
	if (CUR != ':') {
	    xmlChar *prefix = token;
	    xmlNsPtr ns;

	    /*
	     * This is a namespace match
	     */
	    token = xsltScanNCName(ctxt);
	    ns = xmlSearchNs(ctxt->doc, ctxt->elem, prefix);
	    if (ns == NULL) {
		xsltTransformError(NULL, NULL, NULL,
	    "xsltCompileStepPattern : no namespace bound to prefix %s\n",
				 prefix);
		xmlFree(prefix);
		ctxt->error = 1;
		goto error;
	    } else {
		URL = xmlStrdup(ns->href);
	    }
	    xmlFree(prefix);
	    if (token == NULL) {
		if (CUR == '*') {
		    NEXT;
                    if (axis == AXIS_ATTRIBUTE) {
                        PUSH(XSLT_OP_ATTR, NULL, URL, novar);
                    }
                    else {
                        PUSH(XSLT_OP_NS, URL, NULL, novar);
                    }
		} else {
		    xsltTransformError(NULL, NULL, NULL,
			    "xsltCompileStepPattern : Name expected\n");
		    ctxt->error = 1;
		    goto error;
		}
	    } else {
                if (axis == AXIS_ATTRIBUTE) {
                    PUSH(XSLT_OP_ATTR, token, URL, novar);
                }
                else {
                    PUSH(XSLT_OP_ELEM, token, URL, novar);
                }
	    }
	} else {
	    if (axis != 0) {
		xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileStepPattern : NodeTest expected\n");
		ctxt->error = 1;
		goto error;
	    }
	    NEXT;
	    if (xmlStrEqual(token, (const xmlChar *) "child")) {
	        axis = AXIS_CHILD;
	    } else if (xmlStrEqual(token, (const xmlChar *) "attribute")) {
	        axis = AXIS_ATTRIBUTE;
	    } else {
		xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileStepPattern : 'child' or 'attribute' expected\n");
		ctxt->error = 1;
		goto error;
	    }
	    xmlFree(token);
            SKIP_BLANKS;
            token = xsltScanNCName(ctxt);
	    goto parse_node_test;
	}
    } else {
	URI = xsltGetQNameURI(ctxt->elem, &token);
	if (token == NULL) {
	    ctxt->error = 1;
	    goto error;
	}
	if (URI != NULL)
	    URL = xmlStrdup(URI);
        if (axis == AXIS_ATTRIBUTE) {
            PUSH(XSLT_OP_ATTR, token, URL, novar);
        }
        else {
            PUSH(XSLT_OP_ELEM, token, URL, novar);
        }
    }
parse_predicate:
    SKIP_BLANKS;
    level = 0;
    while (CUR == '[') {
	const xmlChar *q;
	xmlChar *ret = NULL;

	level++;
	NEXT;
	q = CUR_PTR;
	while (CUR != 0) {
	    /* Skip over nested predicates */
	    if (CUR == '[')
		level++;
	    else if (CUR == ']') {
		level--;
		if (level == 0)
		    break;
	    } else if (CUR == '"') {
		NEXT;
		while ((CUR != 0) && (CUR != '"'))
		    NEXT;
	    } else if (CUR == '\'') {
		NEXT;
		while ((CUR != 0) && (CUR != '\''))
		    NEXT;
	    }
	    NEXT;
	}
	if (CUR == 0) {
	    xsltTransformError(NULL, NULL, NULL,
		    "xsltCompileStepPattern : ']' expected\n");
	    ctxt->error = 1;
	    return;
        }
	ret = xmlStrndup(q, CUR_PTR - q);
	PUSH(XSLT_OP_PREDICATE, ret, NULL, novar);
	/* push the predicate lower than local test */
	SWAP();
	NEXT;
	SKIP_BLANKS;
    }
    return;
error:
    if (token != NULL)
	xmlFree(token);
    if (name != NULL)
	xmlFree(name);
}