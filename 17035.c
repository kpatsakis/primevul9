static int nft_setelem_parse_data(struct nft_ctx *ctx, struct nft_set *set,
				  struct nft_data_desc *desc,
				  struct nft_data *data,
				  struct nlattr *attr)
{
	u32 dtype;

	if (set->dtype == NFT_DATA_VERDICT)
		dtype = NFT_DATA_VERDICT;
	else
		dtype = NFT_DATA_VALUE;

	desc->type = dtype;
	desc->size = NFT_DATA_VALUE_MAXLEN;
	desc->len = set->dlen;
	desc->flags = NFT_DATA_DESC_SETELEM;

	return nft_data_init(ctx, data, desc, attr);
}