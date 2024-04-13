int nft_chain_validate(const struct nft_ctx *ctx, const struct nft_chain *chain)
{
	struct nft_expr *expr, *last;
	const struct nft_data *data;
	struct nft_rule *rule;
	int err;

	if (ctx->level == NFT_JUMP_STACK_SIZE)
		return -EMLINK;

	list_for_each_entry(rule, &chain->rules, list) {
		if (!nft_is_active_next(ctx->net, rule))
			continue;

		nft_rule_for_each_expr(expr, last, rule) {
			if (!expr->ops->validate)
				continue;

			err = expr->ops->validate(ctx, expr, &data);
			if (err < 0)
				return err;
		}

		cond_resched();
	}

	return 0;
}