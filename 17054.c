static int nf_tables_newrule(struct sk_buff *skb, const struct nfnl_info *info,
			     const struct nlattr * const nla[])
{
	struct nftables_pernet *nft_net = nft_pernet(info->net);
	struct netlink_ext_ack *extack = info->extack;
	unsigned int size, i, n, ulen = 0, usize = 0;
	u8 genmask = nft_genmask_next(info->net);
	struct nft_rule *rule, *old_rule = NULL;
	struct nft_expr_info *expr_info = NULL;
	u8 family = info->nfmsg->nfgen_family;
	struct nft_flow_rule *flow = NULL;
	struct net *net = info->net;
	struct nft_userdata *udata;
	struct nft_table *table;
	struct nft_chain *chain;
	struct nft_trans *trans;
	u64 handle, pos_handle;
	struct nft_expr *expr;
	struct nft_ctx ctx;
	struct nlattr *tmp;
	int err, rem;

	lockdep_assert_held(&nft_net->commit_mutex);

	table = nft_table_lookup(net, nla[NFTA_RULE_TABLE], family, genmask,
				 NETLINK_CB(skb).portid);
	if (IS_ERR(table)) {
		NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_TABLE]);
		return PTR_ERR(table);
	}

	if (nla[NFTA_RULE_CHAIN]) {
		chain = nft_chain_lookup(net, table, nla[NFTA_RULE_CHAIN],
					 genmask);
		if (IS_ERR(chain)) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_CHAIN]);
			return PTR_ERR(chain);
		}
		if (nft_chain_is_bound(chain))
			return -EOPNOTSUPP;

	} else if (nla[NFTA_RULE_CHAIN_ID]) {
		chain = nft_chain_lookup_byid(net, table, nla[NFTA_RULE_CHAIN_ID]);
		if (IS_ERR(chain)) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_CHAIN_ID]);
			return PTR_ERR(chain);
		}
	} else {
		return -EINVAL;
	}

	if (nla[NFTA_RULE_HANDLE]) {
		handle = be64_to_cpu(nla_get_be64(nla[NFTA_RULE_HANDLE]));
		rule = __nft_rule_lookup(chain, handle);
		if (IS_ERR(rule)) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_HANDLE]);
			return PTR_ERR(rule);
		}

		if (info->nlh->nlmsg_flags & NLM_F_EXCL) {
			NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_HANDLE]);
			return -EEXIST;
		}
		if (info->nlh->nlmsg_flags & NLM_F_REPLACE)
			old_rule = rule;
		else
			return -EOPNOTSUPP;
	} else {
		if (!(info->nlh->nlmsg_flags & NLM_F_CREATE) ||
		    info->nlh->nlmsg_flags & NLM_F_REPLACE)
			return -EINVAL;
		handle = nf_tables_alloc_handle(table);

		if (chain->use == UINT_MAX)
			return -EOVERFLOW;

		if (nla[NFTA_RULE_POSITION]) {
			pos_handle = be64_to_cpu(nla_get_be64(nla[NFTA_RULE_POSITION]));
			old_rule = __nft_rule_lookup(chain, pos_handle);
			if (IS_ERR(old_rule)) {
				NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_POSITION]);
				return PTR_ERR(old_rule);
			}
		} else if (nla[NFTA_RULE_POSITION_ID]) {
			old_rule = nft_rule_lookup_byid(net, chain, nla[NFTA_RULE_POSITION_ID]);
			if (IS_ERR(old_rule)) {
				NL_SET_BAD_ATTR(extack, nla[NFTA_RULE_POSITION_ID]);
				return PTR_ERR(old_rule);
			}
		}
	}

	nft_ctx_init(&ctx, net, skb, info->nlh, family, table, chain, nla);

	n = 0;
	size = 0;
	if (nla[NFTA_RULE_EXPRESSIONS]) {
		expr_info = kvmalloc_array(NFT_RULE_MAXEXPRS,
					   sizeof(struct nft_expr_info),
					   GFP_KERNEL);
		if (!expr_info)
			return -ENOMEM;

		nla_for_each_nested(tmp, nla[NFTA_RULE_EXPRESSIONS], rem) {
			err = -EINVAL;
			if (nla_type(tmp) != NFTA_LIST_ELEM)
				goto err_release_expr;
			if (n == NFT_RULE_MAXEXPRS)
				goto err_release_expr;
			err = nf_tables_expr_parse(&ctx, tmp, &expr_info[n]);
			if (err < 0) {
				NL_SET_BAD_ATTR(extack, tmp);
				goto err_release_expr;
			}
			size += expr_info[n].ops->size;
			n++;
		}
	}
	/* Check for overflow of dlen field */
	err = -EFBIG;
	if (size >= 1 << 12)
		goto err_release_expr;

	if (nla[NFTA_RULE_USERDATA]) {
		ulen = nla_len(nla[NFTA_RULE_USERDATA]);
		if (ulen > 0)
			usize = sizeof(struct nft_userdata) + ulen;
	}

	err = -ENOMEM;
	rule = kzalloc(sizeof(*rule) + size + usize, GFP_KERNEL_ACCOUNT);
	if (rule == NULL)
		goto err_release_expr;

	nft_activate_next(net, rule);

	rule->handle = handle;
	rule->dlen   = size;
	rule->udata  = ulen ? 1 : 0;

	if (ulen) {
		udata = nft_userdata(rule);
		udata->len = ulen - 1;
		nla_memcpy(udata->data, nla[NFTA_RULE_USERDATA], ulen);
	}

	expr = nft_expr_first(rule);
	for (i = 0; i < n; i++) {
		err = nf_tables_newexpr(&ctx, &expr_info[i], expr);
		if (err < 0) {
			NL_SET_BAD_ATTR(extack, expr_info[i].attr);
			goto err_release_rule;
		}

		if (expr_info[i].ops->validate)
			nft_validate_state_update(net, NFT_VALIDATE_NEED);

		expr_info[i].ops = NULL;
		expr = nft_expr_next(expr);
	}

	if (chain->flags & NFT_CHAIN_HW_OFFLOAD) {
		flow = nft_flow_rule_create(net, rule);
		if (IS_ERR(flow)) {
			err = PTR_ERR(flow);
			goto err_release_rule;
		}
	}

	if (info->nlh->nlmsg_flags & NLM_F_REPLACE) {
		err = nft_delrule(&ctx, old_rule);
		if (err < 0)
			goto err_destroy_flow_rule;

		trans = nft_trans_rule_add(&ctx, NFT_MSG_NEWRULE, rule);
		if (trans == NULL) {
			err = -ENOMEM;
			goto err_destroy_flow_rule;
		}
		list_add_tail_rcu(&rule->list, &old_rule->list);
	} else {
		trans = nft_trans_rule_add(&ctx, NFT_MSG_NEWRULE, rule);
		if (!trans) {
			err = -ENOMEM;
			goto err_destroy_flow_rule;
		}

		if (info->nlh->nlmsg_flags & NLM_F_APPEND) {
			if (old_rule)
				list_add_rcu(&rule->list, &old_rule->list);
			else
				list_add_tail_rcu(&rule->list, &chain->rules);
		 } else {
			if (old_rule)
				list_add_tail_rcu(&rule->list, &old_rule->list);
			else
				list_add_rcu(&rule->list, &chain->rules);
		}
	}
	kvfree(expr_info);
	chain->use++;

	if (flow)
		nft_trans_flow_rule(trans) = flow;

	if (nft_net->validate_state == NFT_VALIDATE_DO)
		return nft_table_validate(net, table);

	return 0;

err_destroy_flow_rule:
	if (flow)
		nft_flow_rule_destroy(flow);
err_release_rule:
	nf_tables_rule_release(&ctx, rule);
err_release_expr:
	for (i = 0; i < n; i++) {
		if (expr_info[i].ops) {
			module_put(expr_info[i].ops->type->owner);
			if (expr_info[i].ops->type->release_ops)
				expr_info[i].ops->type->release_ops(expr_info[i].ops);
		}
	}
	kvfree(expr_info);

	return err;
}