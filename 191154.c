htmlNodeDump(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur) {
    xmlBufPtr buffer;
    size_t ret;

    if ((buf == NULL) || (cur == NULL))
        return(-1);

    xmlInitParser();
    buffer = xmlBufFromBuffer(buf);
    if (buffer == NULL)
        return(-1);

    ret = htmlBufNodeDumpFormat(buffer, doc, cur, 1);

    xmlBufBackToBuffer(buffer);

    if (ret > INT_MAX)
        return(-1);
    return((int) ret);
}