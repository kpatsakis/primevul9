static int parse_tree_attr_replace(struct ldb_parse_tree *tree, void *private_context)
{
	struct parse_tree_attr_replace_ctx *ctx = private_context;
	switch (tree->operation) {
	case LDB_OP_EQUALITY:
	case LDB_OP_GREATER:
	case LDB_OP_LESS:
	case LDB_OP_APPROX:
		if (ldb_attr_cmp(tree->u.equality.attr, ctx->attr) == 0) {
			tree->u.equality.attr = ctx->replace;
		}
		break;
	case LDB_OP_SUBSTRING:
		if (ldb_attr_cmp(tree->u.substring.attr, ctx->attr) == 0) {
			tree->u.substring.attr = ctx->replace;
		}
		break;
	case LDB_OP_PRESENT:
		if (ldb_attr_cmp(tree->u.present.attr, ctx->attr) == 0) {
			tree->u.present.attr = ctx->replace;
		}
		break;
	case LDB_OP_EXTENDED:
		if (tree->u.extended.attr &&
		    ldb_attr_cmp(tree->u.extended.attr, ctx->attr) == 0) {
			tree->u.extended.attr = ctx->replace;
		}
		break;
	default:
		break;
	}
	return LDB_SUCCESS;
}