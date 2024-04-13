static enum ldb_parse_op ldb_parse_filtertype(TALLOC_CTX *mem_ctx, char **type, char **value, const char **s)
{
	enum ldb_parse_op filter = 0;
	char *name, *val, *k;
	const char *p = *s;
	const char *t, *t1;

	/* retrieve attributetype name */
	t = p;

	if (*p == '@') { /* for internal attributes the first char can be @ */
		p++;
	}

	while ((isascii(*p) && isalnum((unsigned char)*p)) || (*p == '-') || (*p == '.')) { 
		/* attribute names can only be alphanums */
		p++;
	}

	if (*p == ':') { /* but extended searches have : and . chars too */
		p = strstr(p, ":=");
		if (p == NULL) { /* malformed attribute name */
			return 0;
		}
	}

	t1 = p;

	while (isspace((unsigned char)*p)) p++;

	if (!strchr("=<>~:", *p)) {
		return 0;
	}

	/* save name */
	name = (char *)talloc_memdup(mem_ctx, t, t1 - t + 1);
	if (name == NULL) return 0;
	name[t1 - t] = '\0';

	/* retrieve filtertype */

	if (*p == '=') {
		filter = LDB_OP_EQUALITY;
	} else if (*(p + 1) == '=') {
		switch (*p) {
		case '<':
			filter = LDB_OP_LESS;
			p++;
			break;
		case '>':
			filter = LDB_OP_GREATER;
			p++;
			break;
		case '~':
			filter = LDB_OP_APPROX;
			p++;
			break;
		case ':':
			filter = LDB_OP_EXTENDED;
			p++;
			break;
		}
	}
	if (!filter) {
		talloc_free(name);
		return 0;
	}
	p++;

	while (isspace((unsigned char)*p)) p++;

	/* retrieve value */
	t = p;

	while (*p && ((*p != ')') || ((*p == ')') && (*(p - 1) == '\\')))) p++;

	val = (char *)talloc_memdup(mem_ctx, t, p - t + 1);
	if (val == NULL) {
		talloc_free(name);
		return 0;
	}
	val[p - t] = '\0';

	k = &(val[p - t]);

	/* remove trailing spaces from value */
	while ((k > val) && (isspace((unsigned char)*(k - 1)))) k--;
	*k = '\0';

	*type = name;
	*value = val;
	*s = p;
	return filter;
}