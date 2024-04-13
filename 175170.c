void skb_append(struct sk_buff *old, struct sk_buff *newsk, struct sk_buff_head *list)
{
	unsigned long flags;

	spin_lock_irqsave(&list->lock, flags);
	__skb_queue_after(list, old, newsk);
	spin_unlock_irqrestore(&list->lock, flags);
}