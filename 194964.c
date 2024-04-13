static struct nft_trans *nft_trans_alloc(struct nft_ctx *ctx, int msg_type,
					 u32 size)
{
	struct nft_trans *trans;

	trans = kzalloc(sizeof(struct nft_trans) + size, GFP_KERNEL);
	if (trans == NULL)
		return NULL;

	trans->msg_type = msg_type;
	trans->ctx	= *ctx;

	return trans;
}