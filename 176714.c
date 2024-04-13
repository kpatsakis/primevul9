char *ldb_filter_from_tree(TALLOC_CTX *mem_ctx, const struct ldb_parse_tree *tree)
{
	char *s, *s2, *ret;
	unsigned int i;

	if (tree == NULL) {
		return NULL;
	}

	switch (tree->operation) {
	case LDB_OP_AND:
	case LDB_OP_OR:
		ret = talloc_asprintf(mem_ctx, "(%c", tree->operation==LDB_OP_AND?'&':'|');
		if (ret == NULL) return NULL;
		for (i=0;i<tree->u.list.num_elements;i++) {
			s = ldb_filter_from_tree(mem_ctx, tree->u.list.elements[i]);
			if (s == NULL) {
				talloc_free(ret);
				return NULL;
			}
			s2 = talloc_asprintf_append(ret, "%s", s);
			talloc_free(s);
			if (s2 == NULL) {
				talloc_free(ret);
				return NULL;
			}
			ret = s2;
		}
		s = talloc_asprintf_append(ret, ")");
		if (s == NULL) {
			talloc_free(ret);
			return NULL;
		}
		return s;
	case LDB_OP_NOT:
		s = ldb_filter_from_tree(mem_ctx, tree->u.isnot.child);
		if (s == NULL) return NULL;

		ret = talloc_asprintf(mem_ctx, "(!%s)", s);
		talloc_free(s);
		return ret;
	case LDB_OP_EQUALITY:
		s = ldb_binary_encode(mem_ctx, tree->u.equality.value);
		if (s == NULL) return NULL;
		ret = talloc_asprintf(mem_ctx, "(%s=%s)", 
				      tree->u.equality.attr, s);
		talloc_free(s);
		return ret;
	case LDB_OP_SUBSTRING:
		ret = talloc_asprintf(mem_ctx, "(%s=%s", tree->u.substring.attr,
				      tree->u.substring.start_with_wildcard?"*":"");
		if (ret == NULL) return NULL;
		for (i = 0; tree->u.substring.chunks && tree->u.substring.chunks[i]; i++) {
			s2 = ldb_binary_encode(mem_ctx, *(tree->u.substring.chunks[i]));
			if (s2 == NULL) {
				talloc_free(ret);
				return NULL;
			}
			if (tree->u.substring.chunks[i+1] ||
			    tree->u.substring.end_with_wildcard) {
				s = talloc_asprintf_append(ret, "%s*", s2);
			} else {
				s = talloc_asprintf_append(ret, "%s", s2);
			}
			if (s == NULL) {
				talloc_free(ret);
				return NULL;
			}
			ret = s;
		}
		s = talloc_asprintf_append(ret, ")");
		if (s == NULL) {
			talloc_free(ret);
			return NULL;
		}
		ret = s;
		return ret;
	case LDB_OP_GREATER:
		s = ldb_binary_encode(mem_ctx, tree->u.equality.value);
		if (s == NULL) return NULL;
		ret = talloc_asprintf(mem_ctx, "(%s>=%s)", 
				      tree->u.equality.attr, s);
		talloc_free(s);
		return ret;
	case LDB_OP_LESS:
		s = ldb_binary_encode(mem_ctx, tree->u.equality.value);
		if (s == NULL) return NULL;
		ret = talloc_asprintf(mem_ctx, "(%s<=%s)", 
				      tree->u.equality.attr, s);
		talloc_free(s);
		return ret;
	case LDB_OP_PRESENT:
		ret = talloc_asprintf(mem_ctx, "(%s=*)", tree->u.present.attr);
		return ret;
	case LDB_OP_APPROX:
		s = ldb_binary_encode(mem_ctx, tree->u.equality.value);
		if (s == NULL) return NULL;
		ret = talloc_asprintf(mem_ctx, "(%s~=%s)", 
				      tree->u.equality.attr, s);
		talloc_free(s);
		return ret;
	case LDB_OP_EXTENDED:
		s = ldb_binary_encode(mem_ctx, tree->u.extended.value);
		if (s == NULL) return NULL;
		ret = talloc_asprintf(mem_ctx, "(%s%s%s%s:=%s)", 
				      tree->u.extended.attr?tree->u.extended.attr:"", 
				      tree->u.extended.dnAttributes?":dn":"",
				      tree->u.extended.rule_id?":":"", 
				      tree->u.extended.rule_id?tree->u.extended.rule_id:"", 
				      s);
		talloc_free(s);
		return ret;
	}
	
	return NULL;
}