static int nf_tables_fill_rule_info(struct sk_buff *skb, struct net *net,
				    u32 portid, u32 seq, int event,
				    u32 flags, int family,
				    const struct nft_table *table,
				    const struct nft_chain *chain,
				    const struct nft_rule *rule)
{
	struct nlmsghdr *nlh;
	struct nfgenmsg *nfmsg;
	const struct nft_expr *expr, *next;
	struct nlattr *list;
	const struct nft_rule *prule;
	int type = event | NFNL_SUBSYS_NFTABLES << 8;

	nlh = nlmsg_put(skb, portid, seq, type, sizeof(struct nfgenmsg),
			flags);
	if (nlh == NULL)
		goto nla_put_failure;

	nfmsg = nlmsg_data(nlh);
	nfmsg->nfgen_family	= family;
	nfmsg->version		= NFNETLINK_V0;
	nfmsg->res_id		= htons(net->nft.base_seq & 0xffff);

	if (nla_put_string(skb, NFTA_RULE_TABLE, table->name))
		goto nla_put_failure;
	if (nla_put_string(skb, NFTA_RULE_CHAIN, chain->name))
		goto nla_put_failure;
	if (nla_put_be64(skb, NFTA_RULE_HANDLE, cpu_to_be64(rule->handle)))
		goto nla_put_failure;

	if ((event != NFT_MSG_DELRULE) && (rule->list.prev != &chain->rules)) {
		prule = list_entry(rule->list.prev, struct nft_rule, list);
		if (nla_put_be64(skb, NFTA_RULE_POSITION,
				 cpu_to_be64(prule->handle)))
			goto nla_put_failure;
	}

	list = nla_nest_start(skb, NFTA_RULE_EXPRESSIONS);
	if (list == NULL)
		goto nla_put_failure;
	nft_rule_for_each_expr(expr, next, rule) {
		struct nlattr *elem = nla_nest_start(skb, NFTA_LIST_ELEM);
		if (elem == NULL)
			goto nla_put_failure;
		if (nf_tables_fill_expr_info(skb, expr) < 0)
			goto nla_put_failure;
		nla_nest_end(skb, elem);
	}
	nla_nest_end(skb, list);

	if (rule->ulen &&
	    nla_put(skb, NFTA_RULE_USERDATA, rule->ulen, nft_userdata(rule)))
		goto nla_put_failure;

	return nlmsg_end(skb, nlh);

nla_put_failure:
	nlmsg_trim(skb, nlh);
	return -1;
}