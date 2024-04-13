const struct ldb_schema_syntax *ldb_standard_syntax_by_name(struct ldb_context *ldb,
							    const char *syntax)
{
	unsigned int i;
	unsigned num_handlers = sizeof(ldb_standard_syntaxes)/sizeof(ldb_standard_syntaxes[0]);
	/* TODO: should be replaced with a binary search */
	for (i=0;i<num_handlers;i++) {
		if (strcmp(ldb_standard_syntaxes[i].name, syntax) == 0) {
			return &ldb_standard_syntaxes[i];
		}
	}
	return NULL;
}