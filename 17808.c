xmlReadIO(xmlInputReadCallback ioread, xmlInputCloseCallback ioclose,
          void *ioctx, const char *URL, const char *encoding, int options)
{
    xmlParserCtxtPtr ctxt;
    xmlParserInputBufferPtr input;
    xmlParserInputPtr stream;

    if (ioread == NULL)
        return (NULL);

    input = xmlParserInputBufferCreateIO(ioread, ioclose, ioctx,
                                         XML_CHAR_ENCODING_NONE);
    if (input == NULL)
        return (NULL);
    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL) {
        xmlFreeParserInputBuffer(input);
        return (NULL);
    }
    stream = xmlNewIOInputStream(ctxt, input, XML_CHAR_ENCODING_NONE);
    if (stream == NULL) {
        xmlFreeParserInputBuffer(input);
	xmlFreeParserCtxt(ctxt);
        return (NULL);
    }
    inputPush(ctxt, stream);
    return (xmlDoRead(ctxt, URL, encoding, options, 0));
}