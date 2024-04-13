static int nf_tables_dump_set(struct sk_buff *skb, struct netlink_callback *cb)
{
	struct nft_set_dump_ctx *dump_ctx = cb->data;
	struct net *net = sock_net(skb->sk);
	struct nftables_pernet *nft_net;
	struct nft_table *table;
	struct nft_set *set;
	struct nft_set_dump_args args;
	bool set_found = false;
	struct nlmsghdr *nlh;
	struct nlattr *nest;
	u32 portid, seq;
	int event;

	rcu_read_lock();
	nft_net = nft_pernet(net);
	cb->seq = READ_ONCE(nft_net->base_seq);

	list_for_each_entry_rcu(table, &nft_net->tables, list) {
		if (dump_ctx->ctx.family != NFPROTO_UNSPEC &&
		    dump_ctx->ctx.family != table->family)
			continue;

		if (table != dump_ctx->ctx.table)
			continue;

		list_for_each_entry_rcu(set, &table->sets, list) {
			if (set == dump_ctx->set) {
				set_found = true;
				break;
			}
		}
		break;
	}

	if (!set_found) {
		rcu_read_unlock();
		return -ENOENT;
	}

	event  = nfnl_msg_type(NFNL_SUBSYS_NFTABLES, NFT_MSG_NEWSETELEM);
	portid = NETLINK_CB(cb->skb).portid;
	seq    = cb->nlh->nlmsg_seq;

	nlh = nfnl_msg_put(skb, portid, seq, event, NLM_F_MULTI,
			   table->family, NFNETLINK_V0, nft_base_seq(net));
	if (!nlh)
		goto nla_put_failure;

	if (nla_put_string(skb, NFTA_SET_ELEM_LIST_TABLE, table->name))
		goto nla_put_failure;
	if (nla_put_string(skb, NFTA_SET_ELEM_LIST_SET, set->name))
		goto nla_put_failure;

	nest = nla_nest_start_noflag(skb, NFTA_SET_ELEM_LIST_ELEMENTS);
	if (nest == NULL)
		goto nla_put_failure;

	args.cb			= cb;
	args.skb		= skb;
	args.iter.genmask	= nft_genmask_cur(net);
	args.iter.skip		= cb->args[0];
	args.iter.count		= 0;
	args.iter.err		= 0;
	args.iter.fn		= nf_tables_dump_setelem;
	set->ops->walk(&dump_ctx->ctx, set, &args.iter);

	if (!args.iter.err && args.iter.count == cb->args[0])
		args.iter.err = nft_set_catchall_dump(net, skb, set);
	rcu_read_unlock();

	nla_nest_end(skb, nest);
	nlmsg_end(skb, nlh);

	if (args.iter.err && args.iter.err != -EMSGSIZE)
		return args.iter.err;
	if (args.iter.count == cb->args[0])
		return 0;

	cb->args[0] = args.iter.count;
	return skb->len;

nla_put_failure:
	rcu_read_unlock();
	return -ENOSPC;
}