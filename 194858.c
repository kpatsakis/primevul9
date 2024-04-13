static int nf_tables_loop_check_setelem(const struct nft_ctx *ctx,
					const struct nft_set *set,
					const struct nft_set_iter *iter,
					const struct nft_set_elem *elem)
{
	if (elem->flags & NFT_SET_ELEM_INTERVAL_END)
		return 0;

	switch (elem->data.verdict) {
	case NFT_JUMP:
	case NFT_GOTO:
		return nf_tables_check_loops(ctx, elem->data.chain);
	default:
		return 0;
	}
}