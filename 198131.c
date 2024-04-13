xmlRegPrintAtom(FILE *output, xmlRegAtomPtr atom) {
    fprintf(output, " atom: ");
    if (atom == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    if (atom->neg)
        fprintf(output, "not ");
    xmlRegPrintAtomType(output, atom->type);
    xmlRegPrintQuantType(output, atom->quant);
    if (atom->quant == XML_REGEXP_QUANT_RANGE)
	fprintf(output, "%d-%d ", atom->min, atom->max);
    if (atom->type == XML_REGEXP_STRING)
	fprintf(output, "'%s' ", (char *) atom->valuep);
    if (atom->type == XML_REGEXP_CHARVAL)
	fprintf(output, "char %c\n", atom->codepoint);
    else if (atom->type == XML_REGEXP_RANGES) {
	int i;
	fprintf(output, "%d entries\n", atom->nbRanges);
	for (i = 0; i < atom->nbRanges;i++)
	    xmlRegPrintRange(output, atom->ranges[i]);
    } else if (atom->type == XML_REGEXP_SUBREG) {
	fprintf(output, "start %d end %d\n", atom->start->no, atom->stop->no);
    } else {
	fprintf(output, "\n");
    }
}