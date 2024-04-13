xmlHTMLPrintFileContext(xmlParserInputPtr input) {
    const xmlChar *cur, *base;
    int len;
    int n;

    if (input == NULL) return;
    xmlGenericError(xmlGenericErrorContext, "<pre>\n");
    cur = input->cur;
    base = input->base;
    while ((cur > base) && ((*cur == '\n') || (*cur == '\r'))) {
	cur--;
    }
    n = 0;
    while ((n++ < 80) && (cur > base) && (*cur != '\n') && (*cur != '\r'))
        cur--;
    if ((*cur == '\n') || (*cur == '\r')) cur++;
    base = cur;
    n = 0;
    while ((*cur != 0) && (*cur != '\n') && (*cur != '\r') && (n < 79)) {
	len = strlen(buffer);
        snprintf(&buffer[len], sizeof(buffer) - len, "%c",
		    (unsigned char) *cur++);
	n++;
    }
    len = strlen(buffer);
    snprintf(&buffer[len], sizeof(buffer) - len, "\n");
    cur = input->cur;
    while ((*cur == '\n') || (*cur == '\r'))
	cur--;
    n = 0;
    while ((cur != base) && (n++ < 80)) {
	len = strlen(buffer);
        snprintf(&buffer[len], sizeof(buffer) - len, " ");
        base++;
    }
    len = strlen(buffer);
    snprintf(&buffer[len], sizeof(buffer) - len, "^\n");
    xmlHTMLEncodeSend();
    xmlGenericError(xmlGenericErrorContext, "</pre>");
}