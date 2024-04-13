static int nf_tables_bind_check_setelem(const struct nft_ctx *ctx,
					const struct nft_set *set,
					const struct nft_set_iter *iter,
					const struct nft_set_elem *elem)
{
	enum nft_registers dreg;

	dreg = nft_type_to_reg(set->dtype);
	return nft_validate_data_load(ctx, dreg, &elem->data,
				      set->dtype == NFT_DATA_VERDICT ?
				      NFT_DATA_VERDICT : NFT_DATA_VALUE);
}