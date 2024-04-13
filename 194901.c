static int nf_tables_getset(struct sock *nlsk, struct sk_buff *skb,
			    const struct nlmsghdr *nlh,
			    const struct nlattr * const nla[])
{
	const struct nft_set *set;
	struct nft_ctx ctx;
	struct sk_buff *skb2;
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	int err;

	/* Verify existence before starting dump */
	err = nft_ctx_init_from_setattr(&ctx, skb, nlh, nla);
	if (err < 0)
		return err;

	if (nlh->nlmsg_flags & NLM_F_DUMP) {
		struct netlink_dump_control c = {
			.dump = nf_tables_dump_sets,
			.done = nf_tables_dump_sets_done,
		};
		struct nft_ctx *ctx_dump;

		ctx_dump = kmalloc(sizeof(*ctx_dump), GFP_KERNEL);
		if (ctx_dump == NULL)
			return -ENOMEM;

		*ctx_dump = ctx;
		c.data = ctx_dump;

		return netlink_dump_start(nlsk, skb, nlh, &c);
	}

	/* Only accept unspec with dump */
	if (nfmsg->nfgen_family == NFPROTO_UNSPEC)
		return -EAFNOSUPPORT;

	set = nf_tables_set_lookup(ctx.table, nla[NFTA_SET_NAME]);
	if (IS_ERR(set))
		return PTR_ERR(set);
	if (set->flags & NFT_SET_INACTIVE)
		return -ENOENT;

	skb2 = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb2 == NULL)
		return -ENOMEM;

	err = nf_tables_fill_set(skb2, &ctx, set, NFT_MSG_NEWSET, 0);
	if (err < 0)
		goto err;

	return nlmsg_unicast(nlsk, skb2, NETLINK_CB(skb).portid);

err:
	kfree_skb(skb2);
	return err;
}