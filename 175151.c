static void skb_release_head_state(struct sk_buff *skb)
{
	skb_dst_drop(skb);
#ifdef CONFIG_XFRM
	secpath_put(skb->sp);
#endif
	if (skb->destructor) {
		WARN_ON(in_irq());
		skb->destructor(skb);
	}
#if IS_ENABLED(CONFIG_NF_CONNTRACK)
	nf_conntrack_put(skb->nfct);
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	nf_bridge_put(skb->nf_bridge);
#endif
/* XXX: IS this still necessary? - JHS */
#ifdef CONFIG_NET_SCHED
	skb->tc_index = 0;
#ifdef CONFIG_NET_CLS_ACT
	skb->tc_verd = 0;
#endif
#endif
}