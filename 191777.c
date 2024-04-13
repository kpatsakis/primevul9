static struct sk_buff *netlink_to_full_skb(const struct sk_buff *skb,
					   gfp_t gfp_mask)
{
	unsigned int len = skb_end_offset(skb);
	struct sk_buff *new;

	new = alloc_skb(len, gfp_mask);
	if (new == NULL)
		return NULL;

	NETLINK_CB(new).portid = NETLINK_CB(skb).portid;
	NETLINK_CB(new).dst_group = NETLINK_CB(skb).dst_group;
	NETLINK_CB(new).creds = NETLINK_CB(skb).creds;

	skb_put_data(new, skb->data, len);
	return new;
}