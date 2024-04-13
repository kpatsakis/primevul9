int nft_data_init(const struct nft_ctx *ctx, struct nft_data *data,
		  struct nft_data_desc *desc, const struct nlattr *nla)
{
	struct nlattr *tb[NFTA_DATA_MAX + 1];
	int err;

	err = nla_parse_nested(tb, NFTA_DATA_MAX, nla, nft_data_policy);
	if (err < 0)
		return err;

	if (tb[NFTA_DATA_VALUE])
		return nft_value_init(ctx, data, desc, tb[NFTA_DATA_VALUE]);
	if (tb[NFTA_DATA_VERDICT] && ctx != NULL)
		return nft_verdict_init(ctx, data, desc, tb[NFTA_DATA_VERDICT]);
	return -EINVAL;
}