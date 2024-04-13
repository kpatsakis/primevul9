xmlHTMLEncodeSend(void) {
    char *result;

    /*
     * xmlEncodeEntitiesReentrant assumes valid UTF-8, but the buffer might
     * end with a truncated UTF-8 sequence. This is a hack to at least avoid
     * an out-of-bounds read.
     */
    memset(&buffer[sizeof(buffer)-4], 0, 4);
    result = (char *) xmlEncodeEntitiesReentrant(NULL, BAD_CAST buffer);
    if (result) {
	xmlGenericError(xmlGenericErrorContext, "%s", result);
	xmlFree(result);
    }
    buffer[0] = 0;
}