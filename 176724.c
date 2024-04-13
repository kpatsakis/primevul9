struct ldb_parse_tree *ldb_parse_tree_copy_shallow(TALLOC_CTX *mem_ctx,
						   const struct ldb_parse_tree *ot)
{
	unsigned int i;
	struct ldb_parse_tree *nt;

	nt = talloc(mem_ctx, struct ldb_parse_tree);
	if (!nt) {
		return NULL;
	}

	*nt = *ot;

	switch (ot->operation) {
	case LDB_OP_AND:
	case LDB_OP_OR:
		nt->u.list.elements = talloc_array(nt, struct ldb_parse_tree *,
						   ot->u.list.num_elements);
		if (!nt->u.list.elements) {
			talloc_free(nt);
			return NULL;
		}

		for (i=0;i<ot->u.list.num_elements;i++) {
			nt->u.list.elements[i] =
				ldb_parse_tree_copy_shallow(nt->u.list.elements,
						ot->u.list.elements[i]);
			if (!nt->u.list.elements[i]) {
				talloc_free(nt);
				return NULL;
			}
		}
		break;
	case LDB_OP_NOT:
		nt->u.isnot.child = ldb_parse_tree_copy_shallow(nt,
							ot->u.isnot.child);
		if (!nt->u.isnot.child) {
			talloc_free(nt);
			return NULL;
		}
		break;
	case LDB_OP_EQUALITY:
	case LDB_OP_GREATER:
	case LDB_OP_LESS:
	case LDB_OP_APPROX:
	case LDB_OP_SUBSTRING:
	case LDB_OP_PRESENT:
	case LDB_OP_EXTENDED:
		break;
	}

	return nt;
}