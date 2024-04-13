static int nft_verdict_dump(struct sk_buff *skb, const struct nft_data *data)
{
	struct nlattr *nest;

	nest = nla_nest_start(skb, NFTA_DATA_VERDICT);
	if (!nest)
		goto nla_put_failure;

	if (nla_put_be32(skb, NFTA_VERDICT_CODE, htonl(data->verdict)))
		goto nla_put_failure;

	switch (data->verdict) {
	case NFT_JUMP:
	case NFT_GOTO:
		if (nla_put_string(skb, NFTA_VERDICT_CHAIN, data->chain->name))
			goto nla_put_failure;
	}
	nla_nest_end(skb, nest);
	return 0;

nla_put_failure:
	return -1;
}