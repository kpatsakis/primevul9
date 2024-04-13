xmlCreateIOParserCtxt(xmlSAXHandlerPtr sax, void *user_data,
	xmlInputReadCallback   ioread, xmlInputCloseCallback  ioclose,
	void *ioctx, xmlCharEncoding enc) {
    xmlParserCtxtPtr ctxt;
    xmlParserInputPtr inputStream;
    xmlParserInputBufferPtr buf;
    
    if (ioread == NULL) return(NULL);

    buf = xmlParserInputBufferCreateIO(ioread, ioclose, ioctx, enc);
    if (buf == NULL) return(NULL);

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
	xmlFreeParserInputBuffer(buf);
	return(NULL);
    }
    if (sax != NULL) {
#ifdef LIBXML_SAX1_ENABLED
	if (ctxt->sax != (xmlSAXHandlerPtr) &xmlDefaultSAXHandler)
#endif /* LIBXML_SAX1_ENABLED */
	    xmlFree(ctxt->sax);
	ctxt->sax = (xmlSAXHandlerPtr) xmlMalloc(sizeof(xmlSAXHandler));
	if (ctxt->sax == NULL) {
	    xmlErrMemory(ctxt, NULL);
	    xmlFreeParserCtxt(ctxt);
	    return(NULL);
	}
	memset(ctxt->sax, 0, sizeof(xmlSAXHandler));
	if (sax->initialized == XML_SAX2_MAGIC)
	    memcpy(ctxt->sax, sax, sizeof(xmlSAXHandler));
	else
	    memcpy(ctxt->sax, sax, sizeof(xmlSAXHandlerV1));
	if (user_data != NULL)
	    ctxt->userData = user_data;
    }	

    inputStream = xmlNewIOInputStream(ctxt, buf, enc);
    if (inputStream == NULL) {
	xmlFreeParserCtxt(ctxt);
	return(NULL);
    }
    inputPush(ctxt, inputStream);

    return(ctxt);
}