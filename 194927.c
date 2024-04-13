static int nf_tables_set_notify(const struct nft_ctx *ctx,
				const struct nft_set *set,
				int event, gfp_t gfp_flags)
{
	struct sk_buff *skb;
	u32 portid = ctx->portid;
	int err;

	if (!ctx->report &&
	    !nfnetlink_has_listeners(ctx->net, NFNLGRP_NFTABLES))
		return 0;

	err = -ENOBUFS;
	skb = nlmsg_new(NLMSG_GOODSIZE, gfp_flags);
	if (skb == NULL)
		goto err;

	err = nf_tables_fill_set(skb, ctx, set, event, 0);
	if (err < 0) {
		kfree_skb(skb);
		goto err;
	}

	err = nfnetlink_send(skb, ctx->net, portid, NFNLGRP_NFTABLES,
			     ctx->report, gfp_flags);
err:
	if (err < 0)
		nfnetlink_set_err(ctx->net, portid, NFNLGRP_NFTABLES, err);
	return err;
}