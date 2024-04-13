htmlDtdDumpOutput(xmlOutputBufferPtr buf, xmlDocPtr doc,
	          const char *encoding ATTRIBUTE_UNUSED) {
    xmlDtdPtr cur = doc->intSubset;

    if (cur == NULL) {
	htmlSaveErr(XML_SAVE_NO_DOCTYPE, (xmlNodePtr) doc, NULL);
	return;
    }
    xmlOutputBufferWriteString(buf, "<!DOCTYPE ");
    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if (cur->ExternalID != NULL) {
	xmlOutputBufferWriteString(buf, " PUBLIC ");
	xmlBufWriteQuotedString(buf->buffer, cur->ExternalID);
	if (cur->SystemID != NULL) {
	    xmlOutputBufferWriteString(buf, " ");
	    xmlBufWriteQuotedString(buf->buffer, cur->SystemID);
	}
    } else if (cur->SystemID != NULL &&
	       xmlStrcmp(cur->SystemID, BAD_CAST "about:legacy-compat")) {
	xmlOutputBufferWriteString(buf, " SYSTEM ");
	xmlBufWriteQuotedString(buf->buffer, cur->SystemID);
    }
    xmlOutputBufferWriteString(buf, ">\n");
}