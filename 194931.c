nf_tables_delrule_deactivate(struct nft_ctx *ctx, struct nft_rule *rule)
{
	/* You cannot delete the same rule twice */
	if (nft_rule_is_active_next(ctx->net, rule)) {
		nft_rule_deactivate_next(ctx->net, rule);
		ctx->chain->use--;
		return 0;
	}
	return -ENOENT;
}