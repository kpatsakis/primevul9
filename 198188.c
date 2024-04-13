xmlFARegExecSaveInputString(xmlRegExecCtxtPtr exec, const xmlChar *value,
	                    void *data) {
#ifdef DEBUG_PUSH
    printf("saving value: %d:%s\n", exec->inputStackNr, value);
#endif
    if (exec->inputStackMax == 0) {
	exec->inputStackMax = 4;
	exec->inputStack = (xmlRegInputTokenPtr)
	    xmlMalloc(exec->inputStackMax * sizeof(xmlRegInputToken));
	if (exec->inputStack == NULL) {
	    xmlRegexpErrMemory(NULL, "pushing input string");
	    exec->inputStackMax = 0;
	    return;
	}
    } else if (exec->inputStackNr + 1 >= exec->inputStackMax) {
	xmlRegInputTokenPtr tmp;

	exec->inputStackMax *= 2;
	tmp = (xmlRegInputTokenPtr) xmlRealloc(exec->inputStack,
			exec->inputStackMax * sizeof(xmlRegInputToken));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(NULL, "pushing input string");
	    exec->inputStackMax /= 2;
	    return;
	}
	exec->inputStack = tmp;
    }
    exec->inputStack[exec->inputStackNr].value = xmlStrdup(value);
    exec->inputStack[exec->inputStackNr].data = data;
    exec->inputStackNr++;
    exec->inputStack[exec->inputStackNr].value = NULL;
    exec->inputStack[exec->inputStackNr].data = NULL;
}