static int nf_tables_getsetelem(struct sock *nlsk, struct sk_buff *skb,
				const struct nlmsghdr *nlh,
				const struct nlattr * const nla[])
{
	const struct nft_set *set;
	struct nft_ctx ctx;
	int err;

	err = nft_ctx_init_from_elemattr(&ctx, skb, nlh, nla, false);
	if (err < 0)
		return err;

	set = nf_tables_set_lookup(ctx.table, nla[NFTA_SET_ELEM_LIST_SET]);
	if (IS_ERR(set))
		return PTR_ERR(set);
	if (set->flags & NFT_SET_INACTIVE)
		return -ENOENT;

	if (nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.dump = nf_tables_dump_set,
		};
		return netlink_dump_start(nlsk, skb, nlh, &c);
	}
	return -EOPNOTSUPP;
}