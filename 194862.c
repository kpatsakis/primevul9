static int nft_trans_set_add(struct nft_ctx *ctx, int msg_type,
			     struct nft_set *set)
{
	struct nft_trans *trans;

	trans = nft_trans_alloc(ctx, msg_type, sizeof(struct nft_trans_set));
	if (trans == NULL)
		return -ENOMEM;

	if (msg_type == NFT_MSG_NEWSET && ctx->nla[NFTA_SET_ID] != NULL) {
		nft_trans_set_id(trans) =
			ntohl(nla_get_be32(ctx->nla[NFTA_SET_ID]));
		set->flags |= NFT_SET_INACTIVE;
	}
	nft_trans_set(trans) = set;
	list_add_tail(&trans->list, &ctx->net->nft.commit_list);

	return 0;
}