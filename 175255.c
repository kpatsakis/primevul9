static void skb_drop_list(struct sk_buff **listp)
{
	kfree_skb_list(*listp);
	*listp = NULL;
}