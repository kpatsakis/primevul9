xmlFARegDebugExec(xmlRegExecCtxtPtr exec) {
    printf("state: %d:%d:idx %d", exec->state->no, exec->transno, exec->index);
    if (exec->inputStack != NULL) {
	int i;
	printf(": ");
	for (i = 0;(i < 3) && (i < exec->inputStackNr);i++)
	    printf("%s ", (const char *)
	           exec->inputStack[exec->inputStackNr - (i + 1)].value);
    } else {
	printf(": %s", &(exec->inputString[exec->index]));
    }
    printf("\n");
}