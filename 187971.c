static inline void prb_fill_rxhash(struct kbdq_core *pkc,
			struct tpacket3_hdr *ppd)
{
	ppd->hv1.tp_rxhash = skb_get_rxhash(pkc->skb);
}