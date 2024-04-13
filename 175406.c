xmlXPathRegisteredVariablesCleanup(xmlXPathContextPtr ctxt) {
    if (ctxt == NULL)
	return;

    xmlHashFree(ctxt->varHash, xmlXPathFreeObjectEntry);
    ctxt->varHash = NULL;
}