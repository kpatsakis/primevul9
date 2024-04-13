xmlRegPrintCtxt(FILE *output, xmlRegParserCtxtPtr ctxt) {
    int i;

    fprintf(output, " ctxt: ");
    if (ctxt == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    fprintf(output, "'%s' ", ctxt->string);
    if (ctxt->error)
	fprintf(output, "error ");
    if (ctxt->neg)
	fprintf(output, "neg ");
    fprintf(output, "\n");
    fprintf(output, "%d atoms:\n", ctxt->nbAtoms);
    for (i = 0;i < ctxt->nbAtoms; i++) {
	fprintf(output, " %02d ", i);
	xmlRegPrintAtom(output, ctxt->atoms[i]);
    }
    if (ctxt->atom != NULL) {
	fprintf(output, "current atom:\n");
	xmlRegPrintAtom(output, ctxt->atom);
    }
    fprintf(output, "%d states:", ctxt->nbStates);
    if (ctxt->start != NULL)
	fprintf(output, " start: %d", ctxt->start->no);
    if (ctxt->end != NULL)
	fprintf(output, " end: %d", ctxt->end->no);
    fprintf(output, "\n");
    for (i = 0;i < ctxt->nbStates; i++) {
	xmlRegPrintState(output, ctxt->states[i]);
    }
    fprintf(output, "%d counters:\n", ctxt->nbCounters);
    for (i = 0;i < ctxt->nbCounters; i++) {
	fprintf(output, " %d: min %d max %d\n", i, ctxt->counters[i].min,
		                                ctxt->counters[i].max);
    }
}