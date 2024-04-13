static inline int skb_queue_empty(const struct sk_buff_head *list)
{
	return list->next == (struct sk_buff *)list;
}