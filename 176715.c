static struct ldb_parse_tree *ldb_parse_filtercomp(TALLOC_CTX *mem_ctx, const char **s)
{
	struct ldb_parse_tree *ret;
	const char *p = *s;

	while (isspace((unsigned char)*p)) p++;

	switch (*p) {
	case '&':
		ret = ldb_parse_filterlist(mem_ctx, &p);
		break;

	case '|':
		ret = ldb_parse_filterlist(mem_ctx, &p);
		break;

	case '!':
		ret = ldb_parse_not(mem_ctx, &p);
		break;

	case '(':
	case ')':
		return NULL;

	default:
		ret = ldb_parse_simple(mem_ctx, &p);

	}

	*s = p;
	return ret;
}