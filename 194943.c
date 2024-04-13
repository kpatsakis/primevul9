static int nf_tables_newsetelem(struct sock *nlsk, struct sk_buff *skb,
				const struct nlmsghdr *nlh,
				const struct nlattr * const nla[])
{
	struct net *net = sock_net(skb->sk);
	const struct nlattr *attr;
	struct nft_set *set;
	struct nft_ctx ctx;
	int rem, err = 0;

	if (nla[NFTA_SET_ELEM_LIST_ELEMENTS] == NULL)
		return -EINVAL;

	err = nft_ctx_init_from_elemattr(&ctx, skb, nlh, nla, true);
	if (err < 0)
		return err;

	set = nf_tables_set_lookup(ctx.table, nla[NFTA_SET_ELEM_LIST_SET]);
	if (IS_ERR(set)) {
		if (nla[NFTA_SET_ELEM_LIST_SET_ID]) {
			set = nf_tables_set_lookup_byid(net,
					nla[NFTA_SET_ELEM_LIST_SET_ID]);
		}
		if (IS_ERR(set))
			return PTR_ERR(set);
	}

	if (!list_empty(&set->bindings) && set->flags & NFT_SET_CONSTANT)
		return -EBUSY;

	nla_for_each_nested(attr, nla[NFTA_SET_ELEM_LIST_ELEMENTS], rem) {
		err = nft_add_set_elem(&ctx, set, attr);
		if (err < 0)
			break;

		set->nelems++;
	}
	return err;
}