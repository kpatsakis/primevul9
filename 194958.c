static int nf_tables_rule_notify(const struct nft_ctx *ctx,
				 const struct nft_rule *rule,
				 int event)
{
	struct sk_buff *skb;
	int err;

	if (!ctx->report &&
	    !nfnetlink_has_listeners(ctx->net, NFNLGRP_NFTABLES))
		return 0;

	err = -ENOBUFS;
	skb = nlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb == NULL)
		goto err;

	err = nf_tables_fill_rule_info(skb, ctx->net, ctx->portid, ctx->seq,
				       event, 0, ctx->afi->family, ctx->table,
				       ctx->chain, rule);
	if (err < 0) {
		kfree_skb(skb);
		goto err;
	}

	err = nfnetlink_send(skb, ctx->net, ctx->portid, NFNLGRP_NFTABLES,
			     ctx->report, GFP_KERNEL);
err:
	if (err < 0) {
		nfnetlink_set_err(ctx->net, ctx->portid, NFNLGRP_NFTABLES,
				  err);
	}
	return err;
}