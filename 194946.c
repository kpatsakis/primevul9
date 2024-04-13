static int nft_trans_chain_add(struct nft_ctx *ctx, int msg_type)
{
	struct nft_trans *trans;

	trans = nft_trans_alloc(ctx, msg_type, sizeof(struct nft_trans_chain));
	if (trans == NULL)
		return -ENOMEM;

	if (msg_type == NFT_MSG_NEWCHAIN)
		ctx->chain->flags |= NFT_CHAIN_INACTIVE;

	list_add_tail(&trans->list, &ctx->net->nft.commit_list);
	return 0;
}