static inline ktime_t skb_get_ktime(const struct sk_buff *skb)
{
	return skb->tstamp;
}