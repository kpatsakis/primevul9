unsigned char *skb_pull(struct sk_buff *skb, unsigned int len)
{
	return skb_pull_inline(skb, len);
}