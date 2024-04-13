static int nft_delrule_by_chain(struct nft_ctx *ctx)
{
	struct nft_rule *rule;
	int err;

	list_for_each_entry(rule, &ctx->chain->rules, list) {
		err = nft_delrule(ctx, rule);
		if (err < 0)
			return err;
	}
	return 0;
}