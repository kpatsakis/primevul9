static int nf_tables_fill_setelem_info(struct sk_buff *skb,
				       const struct nft_ctx *ctx, u32 seq,
				       u32 portid, int event, u16 flags,
				       const struct nft_set *set,
				       const struct nft_set_elem *elem)
{
	struct nfgenmsg *nfmsg;
	struct nlmsghdr *nlh;
	struct nlattr *nest;
	int err;

	event |= NFNL_SUBSYS_NFTABLES << 8;
	nlh = nlmsg_put(skb, portid, seq, event, sizeof(struct nfgenmsg),
			flags);
	if (nlh == NULL)
		goto nla_put_failure;

	nfmsg = nlmsg_data(nlh);
	nfmsg->nfgen_family	= ctx->afi->family;
	nfmsg->version		= NFNETLINK_V0;
	nfmsg->res_id		= htons(ctx->net->nft.base_seq & 0xffff);

	if (nla_put_string(skb, NFTA_SET_TABLE, ctx->table->name))
		goto nla_put_failure;
	if (nla_put_string(skb, NFTA_SET_NAME, set->name))
		goto nla_put_failure;

	nest = nla_nest_start(skb, NFTA_SET_ELEM_LIST_ELEMENTS);
	if (nest == NULL)
		goto nla_put_failure;

	err = nf_tables_fill_setelem(skb, set, elem);
	if (err < 0)
		goto nla_put_failure;

	nla_nest_end(skb, nest);

	return nlmsg_end(skb, nlh);

nla_put_failure:
	nlmsg_trim(skb, nlh);
	return -1;
}