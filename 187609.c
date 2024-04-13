EXPORTED void xml_partial_response(struct transaction_t *txn,
                                   xmlDocPtr doc, xmlNodePtr node,
                                   unsigned level, xmlBufferPtr *buf)
{
    const char *eol = "\n";
    unsigned n;

    if (!config_httpprettytelemetry) {
        level = 0;
        eol = "";
    }

    /* Start with clean buffer */
    if (!*buf) *buf = xmlBufferCreate();

    if (node) {
        /* Add leading indent to buffer */
        for (n = 0; n < level * MARKUP_INDENT; n++) xmlBufferCCat(*buf, " ");

        /* Dump XML node into buffer */
        xmlNodeDump(*buf, doc, node, level, config_httpprettytelemetry);

        /* Add trailing EOL to buffer */
        xmlBufferCCat(*buf, eol);
    }
    else {
        /* End of chunked XML response */
        xmlNodePtr root = xmlDocGetRootElement(doc);

        /* Add close of root element to buffer */
        xmlBufferCCat(*buf, "</");
        if (root->ns->prefix) {
            xmlBufferCat(*buf, root->ns->prefix);
            xmlBufferCCat(*buf, ":");
        }
        xmlBufferCat(*buf, root->name);
        xmlBufferCCat(*buf, ">");

        /* Add trailing EOL to buffer */
        xmlBufferCCat(*buf, eol);
    }

    if (txn) {
        /* Output the XML buffer */
        write_body(0, txn,
                   (char *) xmlBufferContent(*buf), xmlBufferLength(*buf));

        /* Reset the buffer for next chunk */
        xmlBufferEmpty(*buf);
    }
}