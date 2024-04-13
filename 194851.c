static int nf_tables_deltable(struct sock *nlsk, struct sk_buff *skb,
			      const struct nlmsghdr *nlh,
			      const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nft_af_info *afi;
	struct nft_table *table;
	struct net *net = sock_net(skb->sk);
	int family = nfmsg->nfgen_family;
	struct nft_ctx ctx;

	nft_ctx_init(&ctx, skb, nlh, NULL, NULL, NULL, nla);
	if (family == AF_UNSPEC || nla[NFTA_TABLE_NAME] == NULL)
		return nft_flush(&ctx, family);

	afi = nf_tables_afinfo_lookup(net, family, false);
	if (IS_ERR(afi))
		return PTR_ERR(afi);

	table = nf_tables_table_lookup(afi, nla[NFTA_TABLE_NAME]);
	if (IS_ERR(table))
		return PTR_ERR(table);
	if (table->flags & NFT_TABLE_INACTIVE)
		return -ENOENT;

	ctx.afi = afi;
	ctx.table = table;

	return nft_flush_table(&ctx);
}