static struct ldb_parse_tree *ldb_parse_filter(TALLOC_CTX *mem_ctx, const char **s)
{
	struct ldb_parse_tree *ret;
	const char *p = *s;

	if (*p != '(') {
		return NULL;
	}
	p++;

	ret = ldb_parse_filtercomp(mem_ctx, &p);

	if (*p != ')') {
		return NULL;
	}
	p++;

	while (isspace((unsigned char)*p)) {
		p++;
	}

	*s = p;

	return ret;
}