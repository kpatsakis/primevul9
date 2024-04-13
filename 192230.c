static inline void skb_queue_splice(const struct sk_buff_head *list,
				    struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, (struct sk_buff *) head, head->next);
		head->qlen += list->qlen;
	}
}