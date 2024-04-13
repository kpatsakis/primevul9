xmlExpFreeCtxt(xmlExpCtxtPtr ctxt) {
    if (ctxt == NULL)
        return;
    xmlDictFree(ctxt->dict);
    if (ctxt->table != NULL)
	xmlFree(ctxt->table);
    xmlFree(ctxt);
}