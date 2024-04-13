htmlSaveFileFormat(const char *filename, xmlDocPtr cur,
	           const char *encoding, int format) {
    xmlOutputBufferPtr buf;
    xmlCharEncodingHandlerPtr handler = NULL;
    int ret;

    if ((cur == NULL) || (filename == NULL))
        return(-1);

    xmlInitParser();

    if (encoding != NULL) {
	xmlCharEncoding enc;

	enc = xmlParseCharEncoding(encoding);
	if (enc != XML_CHAR_ENCODING_UTF8) {
	    handler = xmlFindCharEncodingHandler(encoding);
	    if (handler == NULL)
		htmlSaveErr(XML_SAVE_UNKNOWN_ENCODING, NULL, encoding);
	}
        htmlSetMetaEncoding(cur, (const xmlChar *) encoding);
    } else {
	htmlSetMetaEncoding(cur, (const xmlChar *) "UTF-8");

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
    buf = xmlOutputBufferCreateFilename(filename, handler, 0);
    if (buf == NULL) return(0);

    htmlDocContentDumpFormatOutput(buf, cur, encoding, format);

    ret = xmlOutputBufferClose(buf);
    return(ret);
}