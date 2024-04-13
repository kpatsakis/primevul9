xmlXIncludeLoadTxt(xmlXIncludeCtxtPtr ctxt, const xmlChar *url, int nr) {
    xmlParserInputBufferPtr buf;
    xmlNodePtr node;
    xmlURIPtr uri;
    xmlChar *URL;
    int i;
    xmlChar *encoding = NULL;
    xmlCharEncoding enc = (xmlCharEncoding) 0;
    xmlParserCtxtPtr pctxt;
    xmlParserInputPtr inputStream;
    int xinclude_multibyte_fallback_used = 0;

    /* Don't read from stdin. */
    if (xmlStrcmp(url, BAD_CAST "-") == 0)
        url = BAD_CAST "./-";

    /*
     * Check the URL and remove any fragment identifier
     */
    uri = xmlParseURI((const char *)url);
    if (uri == NULL) {
	xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref, XML_XINCLUDE_HREF_URI,
	               "invalid value URI %s\n", url);
	return(-1);
    }
    if (uri->fragment != NULL) {
	xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref, XML_XINCLUDE_TEXT_FRAGMENT,
	               "fragment identifier forbidden for text: %s\n",
		       (const xmlChar *) uri->fragment);
	xmlFreeURI(uri);
	return(-1);
    }
    URL = xmlSaveUri(uri);
    xmlFreeURI(uri);
    if (URL == NULL) {
	xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref, XML_XINCLUDE_HREF_URI,
	               "invalid value URI %s\n", url);
	return(-1);
    }

    /*
     * Handling of references to the local document are done
     * directly through ctxt->doc.
     */
    if (URL[0] == 0) {
	xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	               XML_XINCLUDE_TEXT_DOCUMENT,
		       "text serialization of document not available\n", NULL);
	xmlFree(URL);
	return(-1);
    }

    /*
     * Prevent reloading twice the document.
     */
    for (i = 0; i < ctxt->txtNr; i++) {
	if (xmlStrEqual(URL, ctxt->txturlTab[i])) {
            node = xmlNewText(ctxt->txtTab[i]);
	    goto loaded;
	}
    }
    /*
     * Try to get the encoding if available
     */
    if ((ctxt->incTab[nr] != NULL) && (ctxt->incTab[nr]->ref != NULL)) {
	encoding = xmlGetProp(ctxt->incTab[nr]->ref, XINCLUDE_PARSE_ENCODING);
    }
    if (encoding != NULL) {
	/*
	 * TODO: we should not have to remap to the xmlCharEncoding
	 *       predefined set, a better interface than
	 *       xmlParserInputBufferCreateFilename should allow any
	 *       encoding supported by iconv
	 */
        enc = xmlParseCharEncoding((const char *) encoding);
	if (enc == XML_CHAR_ENCODING_ERROR) {
	    xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
	                   XML_XINCLUDE_UNKNOWN_ENCODING,
			   "encoding %s not supported\n", encoding);
	    xmlFree(encoding);
	    xmlFree(URL);
	    return(-1);
	}
	xmlFree(encoding);
    }

    /*
     * Load it.
     */
    pctxt = xmlNewParserCtxt();
    inputStream = xmlLoadExternalEntity((const char*)URL, NULL, pctxt);
    if(inputStream == NULL) {
	xmlFreeParserCtxt(pctxt);
	xmlFree(URL);
	return(-1);
    }
    buf = inputStream->buf;
    if (buf == NULL) {
	xmlFreeInputStream (inputStream);
	xmlFreeParserCtxt(pctxt);
	xmlFree(URL);
	return(-1);
    }
    if (buf->encoder)
	xmlCharEncCloseFunc(buf->encoder);
    buf->encoder = xmlGetCharEncodingHandler(enc);
    node = xmlNewText(NULL);

    /*
     * Scan all chars from the resource and add the to the node
     */
xinclude_multibyte_fallback:
    while (xmlParserInputBufferRead(buf, 128) > 0) {
	int len;
	const xmlChar *content;

	content = xmlBufContent(buf->buffer);
	len = xmlBufLength(buf->buffer);
	for (i = 0;i < len;) {
	    int cur;
	    int l;

	    cur = xmlStringCurrentChar(NULL, &content[i], &l);
	    if (!IS_CHAR(cur)) {
		/* Handle split multibyte char at buffer boundary */
		if (((len - i) < 4) && (!xinclude_multibyte_fallback_used)) {
		    xinclude_multibyte_fallback_used = 1;
		    xmlBufShrink(buf->buffer, i);
		    goto xinclude_multibyte_fallback;
		} else {
		    xmlXIncludeErr(ctxt, ctxt->incTab[nr]->ref,
				   XML_XINCLUDE_INVALID_CHAR,
				   "%s contains invalid char\n", URL);
		    xmlFreeParserCtxt(pctxt);
		    xmlFreeParserInputBuffer(buf);
		    xmlFree(URL);
		    return(-1);
		}
	    } else {
		xinclude_multibyte_fallback_used = 0;
		xmlNodeAddContentLen(node, &content[i], l);
	    }
	    i += l;
	}
	xmlBufShrink(buf->buffer, len);
    }
    xmlFreeParserCtxt(pctxt);
    xmlXIncludeAddTxt(ctxt, node->content, URL);
    xmlFreeInputStream(inputStream);

loaded:
    /*
     * Add the element as the replacement copy.
     */
    ctxt->incTab[nr]->inc = node;
    xmlFree(URL);
    return(0);
}