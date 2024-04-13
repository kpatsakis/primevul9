static int nf_tables_delrule(struct sock *nlsk, struct sk_buff *skb,
			     const struct nlmsghdr *nlh,
			     const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nft_af_info *afi;
	struct net *net = sock_net(skb->sk);
	struct nft_table *table;
	struct nft_chain *chain = NULL;
	struct nft_rule *rule;
	int family = nfmsg->nfgen_family, err = 0;
	struct nft_ctx ctx;

	afi = nf_tables_afinfo_lookup(net, family, false);
	if (IS_ERR(afi))
		return PTR_ERR(afi);

	table = nf_tables_table_lookup(afi, nla[NFTA_RULE_TABLE]);
	if (IS_ERR(table))
		return PTR_ERR(table);
	if (table->flags & NFT_TABLE_INACTIVE)
		return -ENOENT;

	if (nla[NFTA_RULE_CHAIN]) {
		chain = nf_tables_chain_lookup(table, nla[NFTA_RULE_CHAIN]);
		if (IS_ERR(chain))
			return PTR_ERR(chain);
	}

	nft_ctx_init(&ctx, skb, nlh, afi, table, chain, nla);

	if (chain) {
		if (nla[NFTA_RULE_HANDLE]) {
			rule = nf_tables_rule_lookup(chain,
						     nla[NFTA_RULE_HANDLE]);
			if (IS_ERR(rule))
				return PTR_ERR(rule);

			err = nft_delrule(&ctx, rule);
		} else {
			err = nft_delrule_by_chain(&ctx);
		}
	} else {
		list_for_each_entry(chain, &table->chains, list) {
			ctx.chain = chain;
			err = nft_delrule_by_chain(&ctx);
			if (err < 0)
				break;
		}
	}

	return err;
}