static inline __u32 skb_queue_len(const struct sk_buff_head *list_)
{
	return list_->qlen;
}