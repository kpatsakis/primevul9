xsltReleaseLocalRVTs(xsltTransformContextPtr ctxt, xmlDocPtr base)
{
    xmlDocPtr cur = ctxt->localRVT, tmp;

    while ((cur != NULL) && (cur != base)) {
	if (cur->psvi == (void *) ((long) 1)) {
	    cur = (xmlDocPtr) cur->next;
	} else {
	    tmp = cur;
	    cur = (xmlDocPtr) cur->next;

	    if (tmp == ctxt->localRVT)
		ctxt->localRVT = cur;

	    /*
	    * We need ctxt->localRVTBase for extension instructions
	    * which return values (like EXSLT's function).
	    */
	    if (tmp == ctxt->localRVTBase)
		ctxt->localRVTBase = cur;

	    if (tmp->prev)
		tmp->prev->next = (xmlNodePtr) cur;
	    if (cur)
		cur->prev = tmp->prev;
	    xsltReleaseRVT(ctxt, tmp);
	}
    }
}