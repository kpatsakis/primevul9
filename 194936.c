static int nft_delset(struct nft_ctx *ctx, struct nft_set *set)
{
	int err;

	err = nft_trans_set_add(ctx, NFT_MSG_DELSET, set);
	if (err < 0)
		return err;

	list_del_rcu(&set->list);
	ctx->table->use--;

	return err;
}