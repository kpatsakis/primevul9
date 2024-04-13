static int nf_tables_newtable(struct sock *nlsk, struct sk_buff *skb,
			      const struct nlmsghdr *nlh,
			      const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	const struct nlattr *name;
	struct nft_af_info *afi;
	struct nft_table *table;
	struct net *net = sock_net(skb->sk);
	int family = nfmsg->nfgen_family;
	u32 flags = 0;
	struct nft_ctx ctx;
	int err;

	afi = nf_tables_afinfo_lookup(net, family, true);
	if (IS_ERR(afi))
		return PTR_ERR(afi);

	name = nla[NFTA_TABLE_NAME];
	table = nf_tables_table_lookup(afi, name);
	if (IS_ERR(table)) {
		if (PTR_ERR(table) != -ENOENT)
			return PTR_ERR(table);
		table = NULL;
	}

	if (table != NULL) {
		if (table->flags & NFT_TABLE_INACTIVE)
			return -ENOENT;
		if (nlh->nlmsg_flags & NLM_F_EXCL)
			return -EEXIST;
		if (nlh->nlmsg_flags & NLM_F_REPLACE)
			return -EOPNOTSUPP;

		nft_ctx_init(&ctx, skb, nlh, afi, table, NULL, nla);
		return nf_tables_updtable(&ctx);
	}

	if (nla[NFTA_TABLE_FLAGS]) {
		flags = ntohl(nla_get_be32(nla[NFTA_TABLE_FLAGS]));
		if (flags & ~NFT_TABLE_F_DORMANT)
			return -EINVAL;
	}

	if (!try_module_get(afi->owner))
		return -EAFNOSUPPORT;

	table = kzalloc(sizeof(*table) + nla_len(name), GFP_KERNEL);
	if (table == NULL) {
		module_put(afi->owner);
		return -ENOMEM;
	}

	nla_strlcpy(table->name, name, nla_len(name));
	INIT_LIST_HEAD(&table->chains);
	INIT_LIST_HEAD(&table->sets);
	table->flags = flags;

	nft_ctx_init(&ctx, skb, nlh, afi, table, NULL, nla);
	err = nft_trans_table_add(&ctx, NFT_MSG_NEWTABLE);
	if (err < 0) {
		kfree(table);
		module_put(afi->owner);
		return err;
	}
	list_add_tail_rcu(&table->list, &afi->tables);
	return 0;
}