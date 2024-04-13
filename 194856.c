static int nf_tables_check_loops(const struct nft_ctx *ctx,
				 const struct nft_chain *chain)
{
	const struct nft_rule *rule;
	const struct nft_expr *expr, *last;
	const struct nft_set *set;
	struct nft_set_binding *binding;
	struct nft_set_iter iter;

	if (ctx->chain == chain)
		return -ELOOP;

	list_for_each_entry(rule, &chain->rules, list) {
		nft_rule_for_each_expr(expr, last, rule) {
			const struct nft_data *data = NULL;
			int err;

			if (!expr->ops->validate)
				continue;

			err = expr->ops->validate(ctx, expr, &data);
			if (err < 0)
				return err;

			if (data == NULL)
				continue;

			switch (data->verdict) {
			case NFT_JUMP:
			case NFT_GOTO:
				err = nf_tables_check_loops(ctx, data->chain);
				if (err < 0)
					return err;
			default:
				break;
			}
		}
	}

	list_for_each_entry(set, &ctx->table->sets, list) {
		if (!(set->flags & NFT_SET_MAP) ||
		    set->dtype != NFT_DATA_VERDICT)
			continue;

		list_for_each_entry(binding, &set->bindings, list) {
			if (binding->chain != chain)
				continue;

			iter.skip 	= 0;
			iter.count	= 0;
			iter.err	= 0;
			iter.fn		= nf_tables_loop_check_setelem;

			set->ops->walk(ctx, set, &iter);
			if (iter.err < 0)
				return iter.err;
		}
	}

	return 0;
}