static struct ldb_parse_tree *ldb_parse_extended(struct ldb_parse_tree *ret, 
						 char *attr, char *value)
{
	char *p1, *p2;

	ret->operation = LDB_OP_EXTENDED;
	ret->u.extended.value = ldb_binary_decode(ret, value);
	if (ret->u.extended.value.data == NULL) goto failed;

	p1 = strchr(attr, ':');
	if (p1 == NULL) goto failed;
	p2 = strchr(p1+1, ':');

	*p1 = 0;
	if (p2) *p2 = 0;

	ret->u.extended.attr = attr;
	if (strcmp(p1+1, "dn") == 0) {
		ret->u.extended.dnAttributes = 1;
		if (p2) {
			ret->u.extended.rule_id = talloc_strdup(ret, p2+1);
			if (ret->u.extended.rule_id == NULL) goto failed;
		} else {
			ret->u.extended.rule_id = NULL;
		}
	} else {
		ret->u.extended.dnAttributes = 0;
		ret->u.extended.rule_id = talloc_strdup(ret, p1+1);
		if (ret->u.extended.rule_id == NULL) goto failed;
	}

	return ret;

failed:
	talloc_free(ret);
	return NULL;
}