static struct nft_trans *nft_trans_rule_add(struct nft_ctx *ctx, int msg_type,
					    struct nft_rule *rule)
{
	struct nft_trans *trans;

	trans = nft_trans_alloc(ctx, msg_type, sizeof(struct nft_trans_rule));
	if (trans == NULL)
		return NULL;

	nft_trans_rule(trans) = rule;
	list_add_tail(&trans->list, &ctx->net->nft.commit_list);

	return trans;
}