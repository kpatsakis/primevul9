xmlHTMLPrintFileInfo(xmlParserInputPtr input) {
    int len;
    xmlGenericError(xmlGenericErrorContext, "<p>");

    len = strlen(buffer);
    if (input != NULL) {
	if (input->filename) {
	    snprintf(&buffer[len], sizeof(buffer) - len, "%s:%d: ", input->filename,
		    input->line);
	} else {
	    snprintf(&buffer[len], sizeof(buffer) - len, "Entity: line %d: ", input->line);
	}
    }
    xmlHTMLEncodeSend();
}