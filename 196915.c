static void doXPathQuery(xmlDocPtr doc, const char *query) {
    xmlXPathContextPtr ctxt;
    xmlXPathObjectPtr res;

    ctxt = xmlXPathNewContext(doc);
    if (ctxt == NULL) {
        fprintf(stderr, "Out of memory for XPath\n");
        progresult = XMLLINT_ERR_MEM;
        return;
    }
    ctxt->node = (xmlNodePtr) doc;
    res = xmlXPathEval(BAD_CAST query, ctxt);
    xmlXPathFreeContext(ctxt);

    if (res == NULL) {
        fprintf(stderr, "XPath evaluation failure\n");
        progresult = XMLLINT_ERR_XPATH;
        return;
    }
    doXPathDump(res);
    xmlXPathFreeObject(res);
}