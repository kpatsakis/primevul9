static int nf_tables_delset(struct sock *nlsk, struct sk_buff *skb,
			    const struct nlmsghdr *nlh,
			    const struct nlattr * const nla[])
{
	const struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nft_set *set;
	struct nft_ctx ctx;
	int err;

	if (nfmsg->nfgen_family == NFPROTO_UNSPEC)
		return -EAFNOSUPPORT;
	if (nla[NFTA_SET_TABLE] == NULL)
		return -EINVAL;

	err = nft_ctx_init_from_setattr(&ctx, skb, nlh, nla);
	if (err < 0)
		return err;

	set = nf_tables_set_lookup(ctx.table, nla[NFTA_SET_NAME]);
	if (IS_ERR(set))
		return PTR_ERR(set);
	if (set->flags & NFT_SET_INACTIVE)
		return -ENOENT;
	if (!list_empty(&set->bindings))
		return -EBUSY;

	return nft_delset(&ctx, set);
}