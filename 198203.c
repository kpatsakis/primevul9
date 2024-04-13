xmlRegPrintTrans(FILE *output, xmlRegTransPtr trans) {
    fprintf(output, "  trans: ");
    if (trans == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    if (trans->to < 0) {
	fprintf(output, "removed\n");
	return;
    }
    if (trans->nd != 0) {
	if (trans->nd == 2)
	    fprintf(output, "last not determinist, ");
	else
	    fprintf(output, "not determinist, ");
    }
    if (trans->counter >= 0) {
	fprintf(output, "counted %d, ", trans->counter);
    }
    if (trans->count == REGEXP_ALL_COUNTER) {
	fprintf(output, "all transition, ");
    } else if (trans->count >= 0) {
	fprintf(output, "count based %d, ", trans->count);
    }
    if (trans->atom == NULL) {
	fprintf(output, "epsilon to %d\n", trans->to);
	return;
    }
    if (trans->atom->type == XML_REGEXP_CHARVAL)
	fprintf(output, "char %c ", trans->atom->codepoint);
    fprintf(output, "atom %d, to %d\n", trans->atom->no, trans->to);
}