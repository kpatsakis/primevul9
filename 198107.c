xmlRegexpPrint(FILE *output, xmlRegexpPtr regexp) {
    int i;

    if (output == NULL)
        return;
    fprintf(output, " regexp: ");
    if (regexp == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    fprintf(output, "'%s' ", regexp->string);
    fprintf(output, "\n");
    fprintf(output, "%d atoms:\n", regexp->nbAtoms);
    for (i = 0;i < regexp->nbAtoms; i++) {
	fprintf(output, " %02d ", i);
	xmlRegPrintAtom(output, regexp->atoms[i]);
    }
    fprintf(output, "%d states:", regexp->nbStates);
    fprintf(output, "\n");
    for (i = 0;i < regexp->nbStates; i++) {
	xmlRegPrintState(output, regexp->states[i]);
    }
    fprintf(output, "%d counters:\n", regexp->nbCounters);
    for (i = 0;i < regexp->nbCounters; i++) {
	fprintf(output, " %d: min %d max %d\n", i, regexp->counters[i].min,
		                                regexp->counters[i].max);
    }
}