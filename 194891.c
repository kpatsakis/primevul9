static int nf_tables_newrule(struct sock *nlsk, struct sk_buff *skb,
			     const struct nlmsghdr *nlh,
			     const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nft_af_info *afi;
	struct net *net = sock_net(skb->sk);
	struct nft_table *table;
	struct nft_chain *chain;
	struct nft_rule *rule, *old_rule = NULL;
	struct nft_trans *trans = NULL;
	struct nft_expr *expr;
	struct nft_ctx ctx;
	struct nlattr *tmp;
	unsigned int size, i, n, ulen = 0;
	int err, rem;
	bool create;
	u64 handle, pos_handle;

	create = nlh->nlmsg_flags & NLM_F_CREATE ? true : false;

	afi = nf_tables_afinfo_lookup(net, nfmsg->nfgen_family, create);
	if (IS_ERR(afi))
		return PTR_ERR(afi);

	table = nf_tables_table_lookup(afi, nla[NFTA_RULE_TABLE]);
	if (IS_ERR(table))
		return PTR_ERR(table);

	chain = nf_tables_chain_lookup(table, nla[NFTA_RULE_CHAIN]);
	if (IS_ERR(chain))
		return PTR_ERR(chain);

	if (nla[NFTA_RULE_HANDLE]) {
		handle = be64_to_cpu(nla_get_be64(nla[NFTA_RULE_HANDLE]));
		rule = __nf_tables_rule_lookup(chain, handle);
		if (IS_ERR(rule))
			return PTR_ERR(rule);

		if (nlh->nlmsg_flags & NLM_F_EXCL)
			return -EEXIST;
		if (nlh->nlmsg_flags & NLM_F_REPLACE)
			old_rule = rule;
		else
			return -EOPNOTSUPP;
	} else {
		if (!create || nlh->nlmsg_flags & NLM_F_REPLACE)
			return -EINVAL;
		handle = nf_tables_alloc_handle(table);

		if (chain->use == UINT_MAX)
			return -EOVERFLOW;
	}

	if (nla[NFTA_RULE_POSITION]) {
		if (!(nlh->nlmsg_flags & NLM_F_CREATE))
			return -EOPNOTSUPP;

		pos_handle = be64_to_cpu(nla_get_be64(nla[NFTA_RULE_POSITION]));
		old_rule = __nf_tables_rule_lookup(chain, pos_handle);
		if (IS_ERR(old_rule))
			return PTR_ERR(old_rule);
	}

	nft_ctx_init(&ctx, skb, nlh, afi, table, chain, nla);

	n = 0;
	size = 0;
	if (nla[NFTA_RULE_EXPRESSIONS]) {
		nla_for_each_nested(tmp, nla[NFTA_RULE_EXPRESSIONS], rem) {
			err = -EINVAL;
			if (nla_type(tmp) != NFTA_LIST_ELEM)
				goto err1;
			if (n == NFT_RULE_MAXEXPRS)
				goto err1;
			err = nf_tables_expr_parse(&ctx, tmp, &info[n]);
			if (err < 0)
				goto err1;
			size += info[n].ops->size;
			n++;
		}
	}

	if (nla[NFTA_RULE_USERDATA])
		ulen = nla_len(nla[NFTA_RULE_USERDATA]);

	err = -ENOMEM;
	rule = kzalloc(sizeof(*rule) + size + ulen, GFP_KERNEL);
	if (rule == NULL)
		goto err1;

	nft_rule_activate_next(net, rule);

	rule->handle = handle;
	rule->dlen   = size;
	rule->ulen   = ulen;

	if (ulen)
		nla_memcpy(nft_userdata(rule), nla[NFTA_RULE_USERDATA], ulen);

	expr = nft_expr_first(rule);
	for (i = 0; i < n; i++) {
		err = nf_tables_newexpr(&ctx, &info[i], expr);
		if (err < 0)
			goto err2;
		info[i].ops = NULL;
		expr = nft_expr_next(expr);
	}

	if (nlh->nlmsg_flags & NLM_F_REPLACE) {
		if (nft_rule_is_active_next(net, old_rule)) {
			trans = nft_trans_rule_add(&ctx, NFT_MSG_DELRULE,
						   old_rule);
			if (trans == NULL) {
				err = -ENOMEM;
				goto err2;
			}
			nft_rule_deactivate_next(net, old_rule);
			chain->use--;
			list_add_tail_rcu(&rule->list, &old_rule->list);
		} else {
			err = -ENOENT;
			goto err2;
		}
	} else if (nlh->nlmsg_flags & NLM_F_APPEND)
		if (old_rule)
			list_add_rcu(&rule->list, &old_rule->list);
		else
			list_add_tail_rcu(&rule->list, &chain->rules);
	else {
		if (old_rule)
			list_add_tail_rcu(&rule->list, &old_rule->list);
		else
			list_add_rcu(&rule->list, &chain->rules);
	}

	if (nft_trans_rule_add(&ctx, NFT_MSG_NEWRULE, rule) == NULL) {
		err = -ENOMEM;
		goto err3;
	}
	chain->use++;
	return 0;

err3:
	list_del_rcu(&rule->list);
	if (trans) {
		list_del_rcu(&nft_trans_rule(trans)->list);
		nft_rule_clear(net, nft_trans_rule(trans));
		nft_trans_destroy(trans);
		chain->use++;
	}
err2:
	nf_tables_rule_destroy(&ctx, rule);
err1:
	for (i = 0; i < n; i++) {
		if (info[i].ops != NULL)
			module_put(info[i].ops->type->owner);
	}
	return err;
}