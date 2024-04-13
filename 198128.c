xmlRegFreeExecCtxt(xmlRegExecCtxtPtr exec) {
    if (exec == NULL)
	return;

    if (exec->rollbacks != NULL) {
	if (exec->counts != NULL) {
	    int i;

	    for (i = 0;i < exec->maxRollbacks;i++)
		if (exec->rollbacks[i].counts != NULL)
		    xmlFree(exec->rollbacks[i].counts);
	}
	xmlFree(exec->rollbacks);
    }
    if (exec->counts != NULL)
	xmlFree(exec->counts);
    if (exec->inputStack != NULL) {
	int i;

	for (i = 0;i < exec->inputStackNr;i++) {
	    if (exec->inputStack[i].value != NULL)
		xmlFree(exec->inputStack[i].value);
	}
	xmlFree(exec->inputStack);
    }
    if (exec->errString != NULL)
        xmlFree(exec->errString);
    xmlFree(exec);
}