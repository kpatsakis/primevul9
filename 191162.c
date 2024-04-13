htmlBufNodeDumpFormat(xmlBufPtr buf, xmlDocPtr doc, xmlNodePtr cur,
	           int format) {
    size_t use;
    int ret;
    xmlOutputBufferPtr outbuf;

    if (cur == NULL) {
	return (-1);
    }
    if (buf == NULL) {
	return (-1);
    }
    outbuf = (xmlOutputBufferPtr) xmlMalloc(sizeof(xmlOutputBuffer));
    if (outbuf == NULL) {
        htmlSaveErrMemory("allocating HTML output buffer");
	return (-1);
    }
    memset(outbuf, 0, (size_t) sizeof(xmlOutputBuffer));
    outbuf->buffer = buf;
    outbuf->encoder = NULL;
    outbuf->writecallback = NULL;
    outbuf->closecallback = NULL;
    outbuf->context = NULL;
    outbuf->written = 0;

    use = xmlBufUse(buf);
    htmlNodeDumpFormatOutput(outbuf, doc, cur, NULL, format);
    xmlFree(outbuf);
    ret = xmlBufUse(buf) - use;
    return (ret);
}