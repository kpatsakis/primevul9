static int nft_setelem_parse_key(struct nft_ctx *ctx, struct nft_set *set,
				 struct nft_data *key, struct nlattr *attr)
{
	struct nft_data_desc desc = {
		.type	= NFT_DATA_VALUE,
		.size	= NFT_DATA_VALUE_MAXLEN,
		.len	= set->klen,
	};

	return nft_data_init(ctx, key, &desc, attr);
}