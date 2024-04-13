static int nf_tables_newset(struct sock *nlsk, struct sk_buff *skb,
			    const struct nlmsghdr *nlh,
			    const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	const struct nft_set_ops *ops;
	struct nft_af_info *afi;
	struct net *net = sock_net(skb->sk);
	struct nft_table *table;
	struct nft_set *set;
	struct nft_ctx ctx;
	char name[IFNAMSIZ];
	unsigned int size;
	bool create;
	u32 ktype, dtype, flags, policy;
	struct nft_set_desc desc;
	int err;

	if (nla[NFTA_SET_TABLE] == NULL ||
	    nla[NFTA_SET_NAME] == NULL ||
	    nla[NFTA_SET_KEY_LEN] == NULL ||
	    nla[NFTA_SET_ID] == NULL)
		return -EINVAL;

	memset(&desc, 0, sizeof(desc));

	ktype = NFT_DATA_VALUE;
	if (nla[NFTA_SET_KEY_TYPE] != NULL) {
		ktype = ntohl(nla_get_be32(nla[NFTA_SET_KEY_TYPE]));
		if ((ktype & NFT_DATA_RESERVED_MASK) == NFT_DATA_RESERVED_MASK)
			return -EINVAL;
	}

	desc.klen = ntohl(nla_get_be32(nla[NFTA_SET_KEY_LEN]));
	if (desc.klen == 0 || desc.klen > FIELD_SIZEOF(struct nft_data, data))
		return -EINVAL;

	flags = 0;
	if (nla[NFTA_SET_FLAGS] != NULL) {
		flags = ntohl(nla_get_be32(nla[NFTA_SET_FLAGS]));
		if (flags & ~(NFT_SET_ANONYMOUS | NFT_SET_CONSTANT |
			      NFT_SET_INTERVAL | NFT_SET_MAP))
			return -EINVAL;
	}

	dtype = 0;
	if (nla[NFTA_SET_DATA_TYPE] != NULL) {
		if (!(flags & NFT_SET_MAP))
			return -EINVAL;

		dtype = ntohl(nla_get_be32(nla[NFTA_SET_DATA_TYPE]));
		if ((dtype & NFT_DATA_RESERVED_MASK) == NFT_DATA_RESERVED_MASK &&
		    dtype != NFT_DATA_VERDICT)
			return -EINVAL;

		if (dtype != NFT_DATA_VERDICT) {
			if (nla[NFTA_SET_DATA_LEN] == NULL)
				return -EINVAL;
			desc.dlen = ntohl(nla_get_be32(nla[NFTA_SET_DATA_LEN]));
			if (desc.dlen == 0 ||
			    desc.dlen > FIELD_SIZEOF(struct nft_data, data))
				return -EINVAL;
		} else
			desc.dlen = sizeof(struct nft_data);
	} else if (flags & NFT_SET_MAP)
		return -EINVAL;

	policy = NFT_SET_POL_PERFORMANCE;
	if (nla[NFTA_SET_POLICY] != NULL)
		policy = ntohl(nla_get_be32(nla[NFTA_SET_POLICY]));

	if (nla[NFTA_SET_DESC] != NULL) {
		err = nf_tables_set_desc_parse(&ctx, &desc, nla[NFTA_SET_DESC]);
		if (err < 0)
			return err;
	}

	create = nlh->nlmsg_flags & NLM_F_CREATE ? true : false;

	afi = nf_tables_afinfo_lookup(net, nfmsg->nfgen_family, create);
	if (IS_ERR(afi))
		return PTR_ERR(afi);

	table = nf_tables_table_lookup(afi, nla[NFTA_SET_TABLE]);
	if (IS_ERR(table))
		return PTR_ERR(table);

	nft_ctx_init(&ctx, skb, nlh, afi, table, NULL, nla);

	set = nf_tables_set_lookup(table, nla[NFTA_SET_NAME]);
	if (IS_ERR(set)) {
		if (PTR_ERR(set) != -ENOENT)
			return PTR_ERR(set);
		set = NULL;
	}

	if (set != NULL) {
		if (nlh->nlmsg_flags & NLM_F_EXCL)
			return -EEXIST;
		if (nlh->nlmsg_flags & NLM_F_REPLACE)
			return -EOPNOTSUPP;
		return 0;
	}

	if (!(nlh->nlmsg_flags & NLM_F_CREATE))
		return -ENOENT;

	ops = nft_select_set_ops(nla, &desc, policy);
	if (IS_ERR(ops))
		return PTR_ERR(ops);

	size = 0;
	if (ops->privsize != NULL)
		size = ops->privsize(nla);

	err = -ENOMEM;
	set = kzalloc(sizeof(*set) + size, GFP_KERNEL);
	if (set == NULL)
		goto err1;

	nla_strlcpy(name, nla[NFTA_SET_NAME], sizeof(set->name));
	err = nf_tables_set_alloc_name(&ctx, set, name);
	if (err < 0)
		goto err2;

	INIT_LIST_HEAD(&set->bindings);
	set->ops   = ops;
	set->ktype = ktype;
	set->klen  = desc.klen;
	set->dtype = dtype;
	set->dlen  = desc.dlen;
	set->flags = flags;
	set->size  = desc.size;
	set->policy = policy;

	err = ops->init(set, &desc, nla);
	if (err < 0)
		goto err2;

	err = nft_trans_set_add(&ctx, NFT_MSG_NEWSET, set);
	if (err < 0)
		goto err2;

	list_add_tail_rcu(&set->list, &table->sets);
	table->use++;
	return 0;

err2:
	kfree(set);
err1:
	module_put(ops->owner);
	return err;
}