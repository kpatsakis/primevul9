xmlRegPrintState(FILE *output, xmlRegStatePtr state) {
    int i;

    fprintf(output, " state: ");
    if (state == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    if (state->type == XML_REGEXP_START_STATE)
	fprintf(output, "START ");
    if (state->type == XML_REGEXP_FINAL_STATE)
	fprintf(output, "FINAL ");

    fprintf(output, "%d, %d transitions:\n", state->no, state->nbTrans);
    for (i = 0;i < state->nbTrans; i++) {
	xmlRegPrintTrans(output, &(state->trans[i]));
    }
}