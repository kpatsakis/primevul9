static struct ldb_parse_tree *ldb_parse_filterlist(TALLOC_CTX *mem_ctx, const char **s)
{
	struct ldb_parse_tree *ret, *next;
	enum ldb_parse_op op;
	const char *p = *s;

	switch (*p) {
		case '&':
			op = LDB_OP_AND;
			break;
		case '|':
			op = LDB_OP_OR;
			break;
		default:
			return NULL;
	}
	p++;

	while (isspace((unsigned char)*p)) p++;

	ret = talloc(mem_ctx, struct ldb_parse_tree);
	if (!ret) {
		errno = ENOMEM;
		return NULL;
	}

	ret->operation = op;
	ret->u.list.num_elements = 1;
	ret->u.list.elements = talloc(ret, struct ldb_parse_tree *);
	if (!ret->u.list.elements) {
		errno = ENOMEM;
		talloc_free(ret);
		return NULL;
	}

	ret->u.list.elements[0] = ldb_parse_filter(ret->u.list.elements, &p);
	if (!ret->u.list.elements[0]) {
		talloc_free(ret);
		return NULL;
	}

	while (isspace((unsigned char)*p)) p++;

	while (*p) {
		struct ldb_parse_tree **e;
		if (*p == ')') {
			break;
		}

		next = ldb_parse_filter(ret->u.list.elements, &p);
		if (next == NULL) {
			/* an invalid filter element */
			talloc_free(ret);
			return NULL;
		}
		e = talloc_realloc(ret, ret->u.list.elements, 
				     struct ldb_parse_tree *, 
				     ret->u.list.num_elements + 1);
		if (!e) {
			errno = ENOMEM;
			talloc_free(ret);
			return NULL;
		}
		ret->u.list.elements = e;
		ret->u.list.elements[ret->u.list.num_elements] = next;
		ret->u.list.num_elements++;
		while (isspace((unsigned char)*p)) p++;
	}

	*s = p;

	return ret;
}