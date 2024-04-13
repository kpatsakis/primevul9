htmlNodeDumpFileFormat(FILE *out, xmlDocPtr doc,
	               xmlNodePtr cur, const char *encoding, int format) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    xmlInitParser();

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

    /*
     * save the content to a temp buffer.
     */
    buf = xmlOutputBufferCreateFile(out, handler);
    if (buf == NULL) return(0);

    htmlNodeDumpFormatOutput(buf, doc, cur, encoding, format);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}