static int nft_ctx_init_from_setattr(struct nft_ctx *ctx,
				     const struct sk_buff *skb,
				     const struct nlmsghdr *nlh,
				     const struct nlattr * const nla[])
{
	struct net *net = sock_net(skb->sk);
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nft_af_info *afi = NULL;
	struct nft_table *table = NULL;

	if (nfmsg->nfgen_family != NFPROTO_UNSPEC) {
		afi = nf_tables_afinfo_lookup(net, nfmsg->nfgen_family, false);
		if (IS_ERR(afi))
			return PTR_ERR(afi);
	}

	if (nla[NFTA_SET_TABLE] != NULL) {
		if (afi == NULL)
			return -EAFNOSUPPORT;

		table = nf_tables_table_lookup(afi, nla[NFTA_SET_TABLE]);
		if (IS_ERR(table))
			return PTR_ERR(table);
		if (table->flags & NFT_TABLE_INACTIVE)
			return -ENOENT;
	}

	nft_ctx_init(ctx, skb, nlh, afi, table, NULL, nla);
	return 0;
}