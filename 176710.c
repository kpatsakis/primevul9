void ldb_parse_tree_attr_replace(struct ldb_parse_tree *tree,
				 const char *attr,
				 const char *replace)
{
	struct parse_tree_attr_replace_ctx ctx;

	ctx.attr    = attr;
	ctx.replace = replace;

	ldb_parse_tree_walk(tree, parse_tree_attr_replace, &ctx);
}