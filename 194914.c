static void nf_tables_rule_destroy(const struct nft_ctx *ctx,
				   struct nft_rule *rule)
{
	struct nft_expr *expr;

	/*
	 * Careful: some expressions might not be initialized in case this
	 * is called on error from nf_tables_newrule().
	 */
	expr = nft_expr_first(rule);
	while (expr->ops && expr != nft_expr_last(rule)) {
		nf_tables_expr_destroy(ctx, expr);
		expr = nft_expr_next(expr);
	}
	kfree(rule);
}