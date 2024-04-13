static struct sk_buff *add_grhead(struct sk_buff *skb, struct ip_mc_list *pmc,
	int type, struct igmpv3_grec **ppgr, unsigned int mtu)
{
	struct net_device *dev = pmc->interface->dev;
	struct igmpv3_report *pih;
	struct igmpv3_grec *pgr;

	if (!skb) {
		skb = igmpv3_newpack(dev, mtu);
		if (!skb)
			return NULL;
	}
	pgr = skb_put(skb, sizeof(struct igmpv3_grec));
	pgr->grec_type = type;
	pgr->grec_auxwords = 0;
	pgr->grec_nsrcs = 0;
	pgr->grec_mca = pmc->multiaddr;
	pih = igmpv3_report_hdr(skb);
	pih->ngrec = htons(ntohs(pih->ngrec)+1);
	*ppgr = pgr;
	return skb;
}