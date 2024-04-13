static int nf_tables_setelem_notify(const struct nft_ctx *ctx,
				    const struct nft_set *set,
				    const struct nft_set_elem *elem,
				    int event, u16 flags)
{
	struct net *net = ctx->net;
	u32 portid = ctx->portid;
	struct sk_buff *skb;
	int err;

	if (!ctx->report && !nfnetlink_has_listeners(net, NFNLGRP_NFTABLES))
		return 0;

	err = -ENOBUFS;
	skb = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb == NULL)
		goto err;

	err = nf_tables_fill_setelem_info(skb, ctx, 0, portid, event, flags,
					  set, elem);
	if (err < 0) {
		kfree_skb(skb);
		goto err;
	}

	err = nfnetlink_send(skb, net, portid, NFNLGRP_NFTABLES, ctx->report,
			     GFP_KERNEL);
err:
	if (err < 0)
		nfnetlink_set_err(net, portid, NFNLGRP_NFTABLES, err);
	return err;
}