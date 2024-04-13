void skb_tx_error(struct sk_buff *skb)
{
	if (skb_shinfo(skb)->tx_flags & SKBTX_DEV_ZEROCOPY) {
		struct ubuf_info *uarg;

		uarg = skb_shinfo(skb)->destructor_arg;
		if (uarg->callback)
			uarg->callback(uarg, false);
		skb_shinfo(skb)->tx_flags &= ~SKBTX_DEV_ZEROCOPY;
	}
}