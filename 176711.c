static struct ldb_parse_tree *ldb_parse_not(TALLOC_CTX *mem_ctx, const char **s)
{
	struct ldb_parse_tree *ret;
	const char *p = *s;

	if (*p != '!') {
		return NULL;
	}
	p++;

	ret = talloc(mem_ctx, struct ldb_parse_tree);
	if (!ret) {
		errno = ENOMEM;
		return NULL;
	}

	ret->operation = LDB_OP_NOT;
	ret->u.isnot.child = ldb_parse_filter(ret, &p);
	if (!ret->u.isnot.child) {
		talloc_free(ret);
		return NULL;
	}

	*s = p;

	return ret;
}