int nft_data_init(const struct nft_ctx *ctx, struct nft_data *data,
		  struct nft_data_desc *desc, const struct nlattr *nla)
{
	struct nlattr *tb[NFTA_DATA_MAX + 1];
	int err;

	if (WARN_ON_ONCE(!desc->size))
		return -EINVAL;

	err = nla_parse_nested_deprecated(tb, NFTA_DATA_MAX, nla,
					  nft_data_policy, NULL);
	if (err < 0)
		return err;

	if (tb[NFTA_DATA_VALUE]) {
		if (desc->type != NFT_DATA_VALUE)
			return -EINVAL;

		err = nft_value_init(ctx, data, desc, tb[NFTA_DATA_VALUE]);
	} else if (tb[NFTA_DATA_VERDICT] && ctx != NULL) {
		if (desc->type != NFT_DATA_VERDICT)
			return -EINVAL;

		err = nft_verdict_init(ctx, data, desc, tb[NFTA_DATA_VERDICT]);
	} else {
		err = -EINVAL;
	}

	return err;
}