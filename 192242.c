static inline struct sk_buff *dev_alloc_skb(unsigned int length)
{
	return netdev_alloc_skb(NULL, length);
}