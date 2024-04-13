static inline void yam_rx_flag(struct net_device *dev, struct yam_port *yp)
{
	if (yp->dcd && yp->rx_len >= 3 && yp->rx_len < YAM_MAX_FRAME) {
		int pkt_len = yp->rx_len - 2 + 1;	/* -CRC + kiss */
		struct sk_buff *skb;

		if ((yp->rx_crch & yp->rx_crcl) != 0xFF) {
			/* Bad crc */
		} else {
			if (!(skb = dev_alloc_skb(pkt_len))) {
				printk(KERN_WARNING "%s: memory squeeze, dropping packet\n", dev->name);
				++dev->stats.rx_dropped;
			} else {
				unsigned char *cp;
				cp = skb_put(skb, pkt_len);
				*cp++ = 0;		/* KISS kludge */
				memcpy(cp, yp->rx_buf, pkt_len - 1);
				skb->protocol = ax25_type_trans(skb, dev);
				netif_rx(skb);
				++dev->stats.rx_packets;
			}
		}
	}
	yp->rx_len = 0;
	yp->rx_crcl = 0x21;
	yp->rx_crch = 0xf3;
}