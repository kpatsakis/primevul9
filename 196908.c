xmlHTMLWarning(void *ctx, const char *msg, ...)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlParserInputPtr input;
    va_list args;
    int len;

    buffer[0] = 0;
    input = ctxt->input;
    if ((input != NULL) && (input->filename == NULL) && (ctxt->inputNr > 1)) {
        input = ctxt->inputTab[ctxt->inputNr - 2];
    }


    xmlHTMLPrintFileInfo(input);

    xmlGenericError(xmlGenericErrorContext, "<b>warning</b>: ");
    va_start(args, msg);
    len = strlen(buffer);
    vsnprintf(&buffer[len],  sizeof(buffer) - len, msg, args);
    va_end(args);
    xmlHTMLEncodeSend();
    xmlGenericError(xmlGenericErrorContext, "</p>\n");

    xmlHTMLPrintFileContext(input);
    xmlHTMLEncodeSend();
}