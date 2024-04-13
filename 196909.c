testSAX(const char *filename) {
    xmlSAXHandlerPtr handler;
    const char *user_data = "user_data"; /* mostly for debugging */
    xmlParserInputBufferPtr buf = NULL;
    xmlParserInputPtr inputStream;
    xmlParserCtxtPtr ctxt = NULL;
    xmlSAXHandlerPtr old_sax = NULL;

    callbacks = 0;

    if (noout) {
        handler = emptySAXHandler;
#ifdef LIBXML_SAX1_ENABLED
    } else if (sax1) {
        handler = debugSAXHandler;
#endif
    } else {
        handler = debugSAX2Handler;
    }

    /*
     * it's not the simplest code but the most generic in term of I/O
     */
    buf = xmlParserInputBufferCreateFilename(filename, XML_CHAR_ENCODING_NONE);
    if (buf == NULL) {
        goto error;
    }

#ifdef LIBXML_SCHEMAS_ENABLED
    if (wxschemas != NULL) {
        int ret;
	xmlSchemaValidCtxtPtr vctxt;

	vctxt = xmlSchemaNewValidCtxt(wxschemas);
	xmlSchemaSetValidErrors(vctxt, xmlGenericError, xmlGenericError, NULL);
	xmlSchemaValidateSetFilename(vctxt, filename);

	ret = xmlSchemaValidateStream(vctxt, buf, 0, handler,
	                              (void *)user_data);
	if (repeat == 0) {
	    if (ret == 0) {
	        if (!quiet) {
	            fprintf(stderr, "%s validates\n", filename);
	        }
	    } else if (ret > 0) {
		fprintf(stderr, "%s fails to validate\n", filename);
		progresult = XMLLINT_ERR_VALID;
	    } else {
		fprintf(stderr, "%s validation generated an internal error\n",
		       filename);
		progresult = XMLLINT_ERR_VALID;
	    }
	}
	xmlSchemaFreeValidCtxt(vctxt);
    } else
#endif
    {
	/*
	 * Create the parser context amd hook the input
	 */
	ctxt = xmlNewParserCtxt();
	if (ctxt == NULL) {
	    xmlFreeParserInputBuffer(buf);
	    goto error;
	}
	old_sax = ctxt->sax;
	ctxt->sax = handler;
	ctxt->userData = (void *) user_data;
	inputStream = xmlNewIOInputStream(ctxt, buf, XML_CHAR_ENCODING_NONE);
	if (inputStream == NULL) {
	    xmlFreeParserInputBuffer(buf);
	    goto error;
	}
	inputPush(ctxt, inputStream);

	/* do the parsing */
	xmlParseDocument(ctxt);

	if (ctxt->myDoc != NULL) {
	    fprintf(stderr, "SAX generated a doc !\n");
	    xmlFreeDoc(ctxt->myDoc);
	    ctxt->myDoc = NULL;
	}
    }

error:
    if (ctxt != NULL) {
        ctxt->sax = old_sax;
        xmlFreeParserCtxt(ctxt);
    }
}