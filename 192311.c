static inline int skb_shared(const struct sk_buff *skb)
{
	return atomic_read(&skb->users) != 1;
}