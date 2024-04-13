xmlBufferAddHead(xmlBufferPtr buf, const xmlChar *str, int len) {
    unsigned int needSize;

    if (buf == NULL)
        return(-1);
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return -1;
    if (str == NULL) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAddHead: str == NULL\n");
#endif
	return -1;
    }
    if (len < -1) {
#ifdef DEBUG_BUFFER
        xmlGenericError(xmlGenericErrorContext,
		"xmlBufferAddHead: len < 0\n");
#endif
	return -1;
    }
    if (len == 0) return 0;

    if (len < 0)
        len = xmlStrlen(str);

    if (len <= 0) return -1;

    if ((buf->alloc == XML_BUFFER_ALLOC_IO) && (buf->contentIO != NULL)) {
        size_t start_buf = buf->content - buf->contentIO;

	if (start_buf > (unsigned int) len) {
	    /*
	     * We can add it in the space previously shrinked
	     */
	    buf->content -= len;
            memmove(&buf->content[0], str, len);
	    buf->use += len;
	    buf->size += len;
	    return(0);
	}
    }
    needSize = buf->use + len + 2;
    if (needSize > buf->size){
        if (!xmlBufferResize(buf, needSize)){
	    xmlTreeErrMemory("growing buffer");
            return XML_ERR_NO_MEMORY;
        }
    }

    memmove(&buf->content[len], &buf->content[0], buf->use);
    memmove(&buf->content[0], str, len);
    buf->use += len;
    buf->content[buf->use] = 0;
    return 0;
}