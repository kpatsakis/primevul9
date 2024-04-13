static int nft_trans_table_add(struct nft_ctx *ctx, int msg_type)
{
	struct nft_trans *trans;

	trans = nft_trans_alloc(ctx, msg_type, sizeof(struct nft_trans_table));
	if (trans == NULL)
		return -ENOMEM;

	if (msg_type == NFT_MSG_NEWTABLE)
		ctx->table->flags |= NFT_TABLE_INACTIVE;

	list_add_tail(&trans->list, &ctx->net->nft.commit_list);
	return 0;
}