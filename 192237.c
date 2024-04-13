static inline void nf_reset_trace(struct sk_buff *skb)
{
#if IS_ENABLED(CONFIG_NETFILTER_XT_TARGET_TRACE)
	skb->nf_trace = 0;
#endif
}