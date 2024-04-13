xmlRegStateAddTransTo(xmlRegParserCtxtPtr ctxt, xmlRegStatePtr target,
                      int from) {
    if (target->maxTransTo == 0) {
	target->maxTransTo = 8;
	target->transTo = (int *) xmlMalloc(target->maxTransTo *
		                             sizeof(int));
	if (target->transTo == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding transition");
	    target->maxTransTo = 0;
	    return;
	}
    } else if (target->nbTransTo >= target->maxTransTo) {
	int *tmp;
	target->maxTransTo *= 2;
	tmp = (int *) xmlRealloc(target->transTo, target->maxTransTo *
		                             sizeof(int));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding transition");
	    target->maxTransTo /= 2;
	    return;
	}
	target->transTo = tmp;
    }
    target->transTo[target->nbTransTo] = from;
    target->nbTransTo++;
}