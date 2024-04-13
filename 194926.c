static void nft_ctx_init(struct nft_ctx *ctx,
			 const struct sk_buff *skb,
			 const struct nlmsghdr *nlh,
			 struct nft_af_info *afi,
			 struct nft_table *table,
			 struct nft_chain *chain,
			 const struct nlattr * const *nla)
{
	ctx->net	= sock_net(skb->sk);
	ctx->afi	= afi;
	ctx->table	= table;
	ctx->chain	= chain;
	ctx->nla   	= nla;
	ctx->portid	= NETLINK_CB(skb).portid;
	ctx->report	= nlmsg_report(nlh);
	ctx->seq	= nlh->nlmsg_seq;
}