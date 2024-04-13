xmlParseCheckTransition(xmlParserCtxtPtr ctxt, const char *chunk, int size) {
    if ((ctxt == NULL) || (chunk == NULL) || (size < 0))
        return(-1);
    if (ctxt->instate == XML_PARSER_START_TAG) {
        if (memchr(chunk, '>', size) != NULL)
            return(1);
        return(0);
    }
    if (ctxt->progressive == XML_PARSER_COMMENT) {
        if (memchr(chunk, '>', size) != NULL)
            return(1);
        return(0);
    }
    if (ctxt->instate == XML_PARSER_CDATA_SECTION) {
        if (memchr(chunk, '>', size) != NULL)
            return(1);
        return(0);
    }
    if (ctxt->progressive == XML_PARSER_PI) {
        if (memchr(chunk, '>', size) != NULL)
            return(1);
        return(0);
    }
    if (ctxt->instate == XML_PARSER_END_TAG) {
        if (memchr(chunk, '>', size) != NULL)
            return(1);
        return(0);
    }
    if ((ctxt->progressive == XML_PARSER_DTD) ||
        (ctxt->instate == XML_PARSER_DTD)) {
        if (memchr(chunk, ']', size) != NULL)
            return(1);
        return(0);
    }
    return(1);
}