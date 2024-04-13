static int nft_delchain(struct nft_ctx *ctx)
{
	int err;

	err = nft_trans_chain_add(ctx, NFT_MSG_DELCHAIN);
	if (err < 0)
		return err;

	ctx->table->use--;
	list_del_rcu(&ctx->chain->list);

	return err;
}