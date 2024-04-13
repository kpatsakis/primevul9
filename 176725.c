struct ldb_parse_tree *ldb_parse_tree(TALLOC_CTX *mem_ctx, const char *s)
{
	if (s == NULL || *s == 0) {
		s = "(|(objectClass=*)(distinguishedName=*))";
	}

	while (isspace((unsigned char)*s)) s++;

	if (*s == '(') {
		return ldb_parse_filter(mem_ctx, &s);
	}

	return ldb_parse_simple(mem_ctx, &s);
}