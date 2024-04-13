static inline void nf_reset(struct sk_buff *skb)
{
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	nf_conntrack_put(skb->nfct);
	skb->nfct = NULL;
#endif
#ifdef NET_SKBUFF_NF_DEFRAG_NEEDED
	nf_conntrack_put_reasm(skb->nfct_reasm);
	skb->nfct_reasm = NULL;
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	nf_bridge_put(skb->nf_bridge);
	skb->nf_bridge = NULL;
#endif
}