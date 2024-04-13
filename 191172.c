htmlDocContentDumpFormatOutput(xmlOutputBufferPtr buf, xmlDocPtr cur,
	                       const char *encoding, int format) {
    htmlNodeDumpFormatOutput(buf, cur, (xmlNodePtr) cur, encoding, format);
}