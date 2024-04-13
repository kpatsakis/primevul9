static inline void __net_timestamp(struct sk_buff *skb)
{
	skb->tstamp = ktime_get_real();
}