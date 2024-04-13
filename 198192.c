xmlRegNewExecCtxt(xmlRegexpPtr comp, xmlRegExecCallbacks callback, void *data) {
    xmlRegExecCtxtPtr exec;

    if (comp == NULL)
	return(NULL);
    if ((comp->compact == NULL) && (comp->states == NULL))
        return(NULL);
    exec = (xmlRegExecCtxtPtr) xmlMalloc(sizeof(xmlRegExecCtxt));
    if (exec == NULL) {
	xmlRegexpErrMemory(NULL, "creating execution context");
	return(NULL);
    }
    memset(exec, 0, sizeof(xmlRegExecCtxt));
    exec->inputString = NULL;
    exec->index = 0;
    exec->determinist = 1;
    exec->maxRollbacks = 0;
    exec->nbRollbacks = 0;
    exec->rollbacks = NULL;
    exec->status = 0;
    exec->comp = comp;
    if (comp->compact == NULL)
	exec->state = comp->states[0];
    exec->transno = 0;
    exec->transcount = 0;
    exec->callback = callback;
    exec->data = data;
    if (comp->nbCounters > 0) {
        /*
	 * For error handling, exec->counts is allocated twice the size
	 * the second half is used to store the data in case of rollback
	 */
	exec->counts = (int *) xmlMalloc(comp->nbCounters * sizeof(int)
	                                 * 2);
	if (exec->counts == NULL) {
	    xmlRegexpErrMemory(NULL, "creating execution context");
	    xmlFree(exec);
	    return(NULL);
	}
        memset(exec->counts, 0, comp->nbCounters * sizeof(int) * 2);
	exec->errCounts = &exec->counts[comp->nbCounters];
    } else {
	exec->counts = NULL;
	exec->errCounts = NULL;
    }
    exec->inputStackMax = 0;
    exec->inputStackNr = 0;
    exec->inputStack = NULL;
    exec->errStateNo = -1;
    exec->errString = NULL;
    exec->nbPush = 0;
    return(exec);
}