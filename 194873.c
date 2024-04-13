static int nf_tables_getchain(struct sock *nlsk, struct sk_buff *skb,
			      const struct nlmsghdr *nlh,
			      const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	const struct nft_af_info *afi;
	const struct nft_table *table;
	const struct nft_chain *chain;
	struct sk_buff *skb2;
	struct net *net = sock_net(skb->sk);
	int family = nfmsg->nfgen_family;
	int err;

	if (nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.dump = nf_tables_dump_chains,
		};
		return netlink_dump_start(nlsk, skb, nlh, &c);
	}

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

	skb2 = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!skb2)
		return -ENOMEM;

	err = nf_tables_fill_chain_info(skb2, net, NETLINK_CB(skb).portid,
					nlh->nlmsg_seq, NFT_MSG_NEWCHAIN, 0,
					family, table, chain);
	if (err < 0)
		goto err;

	return nlmsg_unicast(nlsk, skb2, NETLINK_CB(skb).portid);

err:
	kfree_skb(skb2);
	return err;
}