xmlRegGetCounter(xmlRegParserCtxtPtr ctxt) {
    if (ctxt->maxCounters == 0) {
	ctxt->maxCounters = 4;
	ctxt->counters = (xmlRegCounter *) xmlMalloc(ctxt->maxCounters *
		                             sizeof(xmlRegCounter));
	if (ctxt->counters == NULL) {
	    xmlRegexpErrMemory(ctxt, "allocating counter");
	    ctxt->maxCounters = 0;
	    return(-1);
	}
    } else if (ctxt->nbCounters >= ctxt->maxCounters) {
	xmlRegCounter *tmp;
	ctxt->maxCounters *= 2;
	tmp = (xmlRegCounter *) xmlRealloc(ctxt->counters, ctxt->maxCounters *
		                           sizeof(xmlRegCounter));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(ctxt, "allocating counter");
	    ctxt->maxCounters /= 2;
	    return(-1);
	}
	ctxt->counters = tmp;
    }
    ctxt->counters[ctxt->nbCounters].min = -1;
    ctxt->counters[ctxt->nbCounters].max = -1;
    return(ctxt->nbCounters++);
}