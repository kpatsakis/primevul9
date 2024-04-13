static inline void __skb_unlink(struct sk_buff *skb, struct sk_buff_head *list)
{
	struct sk_buff *next, *prev;

	list->qlen--;
	next	   = skb->next;
	prev	   = skb->prev;
	skb->next  = skb->prev = NULL;
	next->prev = prev;
	prev->next = next;
}