static int nft_value_init(const struct nft_ctx *ctx, struct nft_data *data,
			  struct nft_data_desc *desc, const struct nlattr *nla)
{
	unsigned int len;

	len = nla_len(nla);
	if (len == 0)
		return -EINVAL;
	if (len > sizeof(data->data))
		return -EOVERFLOW;

	nla_memcpy(data->data, nla, sizeof(data->data));
	desc->type = NFT_DATA_VALUE;
	desc->len  = len;
	return 0;
}