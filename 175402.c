xmlXPathRegisteredNsCleanup(xmlXPathContextPtr ctxt) {
    if (ctxt == NULL)
	return;

    xmlHashFree(ctxt->nsHash, xmlHashDefaultDeallocator);
    ctxt->nsHash = NULL;
}