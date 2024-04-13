static int nf_tables_newchain(struct sk_buff *skb, const struct nfnl_info *info,
			      const struct nlattr * const nla[])
{
	struct nftables_pernet *nft_net = nft_pernet(info->net);
	struct netlink_ext_ack *extack = info->extack;
	u8 genmask = nft_genmask_next(info->net);
	u8 family = info->nfmsg->nfgen_family;
	struct nft_chain *chain = NULL;
	struct net *net = info->net;
	const struct nlattr *attr;
	struct nft_table *table;
	u8 policy = NF_ACCEPT;
	struct nft_ctx ctx;
	u64 handle = 0;
	u32 flags = 0;

	lockdep_assert_held(&nft_net->commit_mutex);

	table = nft_table_lookup(net, nla[NFTA_CHAIN_TABLE], family, genmask,
				 NETLINK_CB(skb).portid);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_TABLE]);
		return PTR_ERR(table);
	}

	chain = NULL;
	attr = nla[NFTA_CHAIN_NAME];

	if (nla[NFTA_CHAIN_HANDLE]) {
		handle = be64_to_cpu(nla_get_be64(nla[NFTA_CHAIN_HANDLE]));
		chain = nft_chain_lookup_byhandle(table, handle, genmask);
		if (IS_ERR(chain)) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_HANDLE]);
			return PTR_ERR(chain);
		}
		attr = nla[NFTA_CHAIN_HANDLE];
	} else if (nla[NFTA_CHAIN_NAME]) {
		chain = nft_chain_lookup(net, table, attr, genmask);
		if (IS_ERR(chain)) {
			if (PTR_ERR(chain) != -ENOENT) {
				NL_SET_BAD_ATTR(extack, attr);
				return PTR_ERR(chain);
			}
			chain = NULL;
		}
	} else if (!nla[NFTA_CHAIN_ID]) {
		return -EINVAL;
	}

	if (nla[NFTA_CHAIN_POLICY]) {
		if (chain != NULL &&
		    !nft_is_base_chain(chain)) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_POLICY]);
			return -EOPNOTSUPP;
		}

		if (chain == NULL &&
		    nla[NFTA_CHAIN_HOOK] == NULL) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_CHAIN_POLICY]);
			return -EOPNOTSUPP;
		}

		policy = ntohl(nla_get_be32(nla[NFTA_CHAIN_POLICY]));
		switch (policy) {
		case NF_DROP:
		case NF_ACCEPT:
			break;
		default:
			return -EINVAL;
		}
	}

	if (nla[NFTA_CHAIN_FLAGS])
		flags = ntohl(nla_get_be32(nla[NFTA_CHAIN_FLAGS]));
	else if (chain)
		flags = chain->flags;

	if (flags & ~NFT_CHAIN_FLAGS)
		return -EOPNOTSUPP;

	nft_ctx_init(&ctx, net, skb, info->nlh, family, table, chain, nla);

	if (chain != NULL) {
		if (chain->flags & NFT_CHAIN_BINDING)
			return -EINVAL;

		if (info->nlh->nlmsg_flags & NLM_F_EXCL) {
			NL_SET_BAD_ATTR(extack, attr);
			return -EEXIST;
		}
		if (info->nlh->nlmsg_flags & NLM_F_REPLACE)
			return -EOPNOTSUPP;

		flags |= chain->flags & NFT_CHAIN_BASE;
		return nf_tables_updchain(&ctx, genmask, policy, flags, attr,
					  extack);
	}

	return nf_tables_addchain(&ctx, family, genmask, policy, flags, extack);
}