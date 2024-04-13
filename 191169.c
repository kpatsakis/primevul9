htmlDocContentDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr cur,
	                 const char *encoding) {
    htmlNodeDumpFormatOutput(buf, cur, (xmlNodePtr) cur, encoding, 1);
}