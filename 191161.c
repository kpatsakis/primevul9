htmlNodeDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	           xmlNodePtr cur, const char *encoding) {
    htmlNodeDumpFormatOutput(buf, doc, cur, encoding, 1);
}