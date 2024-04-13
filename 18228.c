static int abbrev_cmp(const void *a, const void *b) {
	const RzBinDwarfAbbrevDecl *first = a;
	const RzBinDwarfAbbrevDecl *second = b;

	if (first->offset > second->offset) {
		return 1;
	} else if (first->offset < second->offset) {
		return -1;
	} else {
		return 0;
	}
}