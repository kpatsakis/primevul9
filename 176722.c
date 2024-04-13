int ldb_parse_tree_walk(struct ldb_parse_tree *tree,
			int (*callback)(struct ldb_parse_tree *tree, void *),
			void *private_context)
{
	unsigned int i;
	int ret;

	ret = callback(tree, private_context);
	if (ret != LDB_SUCCESS) {
		return ret;
	}

	switch (tree->operation) {
	case LDB_OP_AND:
	case LDB_OP_OR:
		for (i=0;i<tree->u.list.num_elements;i++) {
			ret = ldb_parse_tree_walk(tree->u.list.elements[i], callback, private_context);
			if (ret != LDB_SUCCESS) {
				return ret;
			}
		}
		break;
	case LDB_OP_NOT:
		ret = ldb_parse_tree_walk(tree->u.isnot.child, callback, private_context);
		if (ret != LDB_SUCCESS) {
			return ret;
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
	return LDB_SUCCESS;
}