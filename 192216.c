static inline void nf_conntrack_get_reasm(struct sk_buff *skb)
{
	if (skb)
		atomic_inc(&skb->users);
}