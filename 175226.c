static int skb_maybe_pull_tail(struct sk_buff *skb, unsigned int len,
			       unsigned int max)
{
	if (skb_headlen(skb) >= len)
		return 0;

	/* If we need to pullup then pullup to the max, so we
	 * won't need to do it again.
	 */
	if (max > skb->len)
		max = skb->len;

	if (__pskb_pull_tail(skb, max - skb_headlen(skb)) == NULL)
		return -ENOMEM;

	if (skb_headlen(skb) < len)
		return -EPROTO;

	return 0;
}