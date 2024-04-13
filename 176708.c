static struct ldb_val **ldb_wildcard_decode(TALLOC_CTX *mem_ctx, const char *string)
{
	struct ldb_val **ret = NULL;
	unsigned int val = 0;
	char *wc, *str;

	wc = talloc_strdup(mem_ctx, string);
	if (wc == NULL) return NULL;

	while (wc && *wc) {
		str = wc;
		wc = ldb_parse_find_wildcard(str);
		if (wc && *wc) {
			if (wc == str) {
				wc++;
				continue;
			}
			*wc = 0;
			wc++;
		}

		ret = talloc_realloc(mem_ctx, ret, struct ldb_val *, val + 2);
		if (ret == NULL) return NULL;

		ret[val] = talloc(mem_ctx, struct ldb_val);
		if (ret[val] == NULL) return NULL;

		*(ret[val]) = ldb_binary_decode(mem_ctx, str);
		if ((ret[val])->data == NULL) return NULL;

		val++;
	}

	if (ret != NULL) {
		ret[val] = NULL;
	}

	return ret;
}