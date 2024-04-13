static int nft_verdict_init(const struct nft_ctx *ctx, struct nft_data *data,
			    struct nft_data_desc *desc, const struct nlattr *nla)
{
	struct nlattr *tb[NFTA_VERDICT_MAX + 1];
	struct nft_chain *chain;
	int err;

	err = nla_parse_nested(tb, NFTA_VERDICT_MAX, nla, nft_verdict_policy);
	if (err < 0)
		return err;

	if (!tb[NFTA_VERDICT_CODE])
		return -EINVAL;
	data->verdict = ntohl(nla_get_be32(tb[NFTA_VERDICT_CODE]));

	switch (data->verdict) {
	default:
		switch (data->verdict & NF_VERDICT_MASK) {
		case NF_ACCEPT:
		case NF_DROP:
		case NF_QUEUE:
			break;
		default:
			return -EINVAL;
		}
		/* fall through */
	case NFT_CONTINUE:
	case NFT_BREAK:
	case NFT_RETURN:
		desc->len = sizeof(data->verdict);
		break;
	case NFT_JUMP:
	case NFT_GOTO:
		if (!tb[NFTA_VERDICT_CHAIN])
			return -EINVAL;
		chain = nf_tables_chain_lookup(ctx->table,
					       tb[NFTA_VERDICT_CHAIN]);
		if (IS_ERR(chain))
			return PTR_ERR(chain);
		if (chain->flags & NFT_BASE_CHAIN)
			return -EOPNOTSUPP;

		chain->use++;
		data->chain = chain;
		desc->len = sizeof(data);
		break;
	}

	desc->type = NFT_DATA_VERDICT;
	return 0;
}