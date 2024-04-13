static int nf_tables_dump_set(struct sk_buff *skb, struct netlink_callback *cb)
{
	const struct nft_set *set;
	struct nft_set_dump_args args;
	struct nft_ctx ctx;
	struct nlattr *nla[NFTA_SET_ELEM_LIST_MAX + 1];
	struct nfgenmsg *nfmsg;
	struct nlmsghdr *nlh;
	struct nlattr *nest;
	u32 portid, seq;
	int event, err;

	err = nlmsg_parse(cb->nlh, sizeof(struct nfgenmsg), nla,
			  NFTA_SET_ELEM_LIST_MAX, nft_set_elem_list_policy);
	if (err < 0)
		return err;

	err = nft_ctx_init_from_elemattr(&ctx, cb->skb, cb->nlh, (void *)nla,
					 false);
	if (err < 0)
		return err;

	set = nf_tables_set_lookup(ctx.table, nla[NFTA_SET_ELEM_LIST_SET]);
	if (IS_ERR(set))
		return PTR_ERR(set);
	if (set->flags & NFT_SET_INACTIVE)
		return -ENOENT;

	event  = NFT_MSG_NEWSETELEM;
	event |= NFNL_SUBSYS_NFTABLES << 8;
	portid = NETLINK_CB(cb->skb).portid;
	seq    = cb->nlh->nlmsg_seq;

	nlh = nlmsg_put(skb, portid, seq, event, sizeof(struct nfgenmsg),
			NLM_F_MULTI);
	if (nlh == NULL)
		goto nla_put_failure;

	nfmsg = nlmsg_data(nlh);
	nfmsg->nfgen_family = ctx.afi->family;
	nfmsg->version      = NFNETLINK_V0;
	nfmsg->res_id	    = htons(ctx.net->nft.base_seq & 0xffff);

	if (nla_put_string(skb, NFTA_SET_ELEM_LIST_TABLE, ctx.table->name))
		goto nla_put_failure;
	if (nla_put_string(skb, NFTA_SET_ELEM_LIST_SET, set->name))
		goto nla_put_failure;

	nest = nla_nest_start(skb, NFTA_SET_ELEM_LIST_ELEMENTS);
	if (nest == NULL)
		goto nla_put_failure;

	args.cb		= cb;
	args.skb	= skb;
	args.iter.skip	= cb->args[0];
	args.iter.count	= 0;
	args.iter.err   = 0;
	args.iter.fn	= nf_tables_dump_setelem;
	set->ops->walk(&ctx, set, &args.iter);

	nla_nest_end(skb, nest);
	nlmsg_end(skb, nlh);

	if (args.iter.err && args.iter.err != -EMSGSIZE)
		return args.iter.err;
	if (args.iter.count == cb->args[0])
		return 0;

	cb->args[0] = args.iter.count;
	return skb->len;

nla_put_failure:
	return -ENOSPC;
}