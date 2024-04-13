void skb_trim(struct sk_buff *skb, unsigned int len)
{
	if (skb->len > len)
		__skb_trim(skb, len);
}