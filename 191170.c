htmlDocDumpMemoryFormat(xmlDocPtr cur, xmlChar**mem, int *size, int format) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    const char *encoding;

    xmlInitParser();

    if ((mem == NULL) || (size == NULL))
        return;
    if (cur == NULL) {
	*mem = NULL;
	*size = 0;
	return;
    }

    encoding = (const char *) htmlGetMetaEncoding(cur);

    if (encoding != NULL) {
	xmlCharEncoding enc;

	enc = xmlParseCharEncoding(encoding);
	if (enc != XML_CHAR_ENCODING_UTF8) {
	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL)
                htmlSaveErr(XML_SAVE_UNKNOWN_ENCODING, NULL, encoding);

	}
    } else {
        /*
         * Fallback to HTML or ASCII when the encoding is unspecified
         */
        if (handler == NULL)
            handler = xmlFindCharEncodingHandler("HTML");
        if (handler == NULL)
            handler = xmlFindCharEncodingHandler("ascii");
    }

    buf = xmlAllocOutputBufferInternal(handler);
    if (buf == NULL) {
	*mem = NULL;
	*size = 0;
	return;
    }

    htmlDocContentDumpFormatOutput(buf, cur, NULL, format);

    xmlOutputBufferFlush(buf);
    if (buf->conv != NULL) {
	*size = xmlBufUse(buf->conv);
	*mem = xmlStrndup(xmlBufContent(buf->conv), *size);
    } else {
	*size = xmlBufUse(buf->buffer);
	*mem = xmlStrndup(xmlBufContent(buf->buffer), *size);
    }
    (void)xmlOutputBufferClose(buf);
}