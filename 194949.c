static int nf_tables_delchain(struct sock *nlsk, struct sk_buff *skb,
			      const struct nlmsghdr *nlh,
			      const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nft_af_info *afi;
	struct nft_table *table;
	struct nft_chain *chain;
	struct net *net = sock_net(skb->sk);
	int family = nfmsg->nfgen_family;
	struct nft_ctx ctx;

	afi = nf_tables_afinfo_lookup(net, family, false);
	if (IS_ERR(afi))
		return PTR_ERR(afi);

	table = nf_tables_table_lookup(afi, nla[NFTA_CHAIN_TABLE]);
	if (IS_ERR(table))
		return PTR_ERR(table);
	if (table->flags & NFT_TABLE_INACTIVE)
		return -ENOENT;

	chain = nf_tables_chain_lookup(table, nla[NFTA_CHAIN_NAME]);
	if (IS_ERR(chain))
		return PTR_ERR(chain);
	if (chain->flags & NFT_CHAIN_INACTIVE)
		return -ENOENT;
	if (chain->use > 0)
		return -EBUSY;

	nft_ctx_init(&ctx, skb, nlh, afi, table, chain, nla);

	return nft_delchain(&ctx);
}