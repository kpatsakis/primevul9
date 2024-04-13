htmlSaveFile(const char *filename, xmlDocPtr cur) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    const char *encoding;
    int ret;

    if ((cur == NULL) || (filename == NULL))
        return(-1);

    xmlInitParser();

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

    /*
     * save the content to a temp buffer.
     */
    buf = xmlOutputBufferCreateFilename(filename, handler, cur->compression);
    if (buf == NULL) return(0);

    htmlDocContentDumpOutput(buf, cur, NULL);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}