int __xfrm_route_forward(struct sk_buff *skb, unsigned short family)
{
	struct net *net = dev_net(skb->dev);
	struct flowi fl;
	struct dst_entry *dst;
	int res = 1;

	if (xfrm_decode_session(skb, &fl, family) < 0) {
		XFRM_INC_STATS(net, LINUX_MIB_XFRMFWDHDRERROR);
		return 0;
	}

	skb_dst_force(skb);
	if (!skb_dst(skb)) {
		XFRM_INC_STATS(net, LINUX_MIB_XFRMFWDHDRERROR);
		return 0;
	}

	dst = xfrm_lookup(net, skb_dst(skb), &fl, NULL, XFRM_LOOKUP_QUEUE);
	if (IS_ERR(dst)) {
		res = 0;
		dst = NULL;
	}
	skb_dst_set(skb, dst);
	return res;
}