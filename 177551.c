isdn_receive_skb_callback(int di, int channel, struct sk_buff *skb)
{
	int i;

	if ((i = isdn_dc2minor(di, channel)) == -1) {
		dev_kfree_skb(skb);
		return;
	}
	/* Update statistics */
	dev->ibytes[i] += skb->len;

	/* First, try to deliver data to network-device */
	if (isdn_net_rcv_skb(i, skb))
		return;

	/* V.110 handling
	 * makes sense for async streams only, so it is
	 * called after possible net-device delivery.
	 */
	if (dev->v110[i]) {
		atomic_inc(&dev->v110use[i]);
		skb = isdn_v110_decode(dev->v110[i], skb);
		atomic_dec(&dev->v110use[i]);
		if (!skb)
			return;
	}

	/* No network-device found, deliver to tty or raw-channel */
	if (skb->len) {
		if (isdn_tty_rcv_skb(i, di, channel, skb))
			return;
		wake_up_interruptible(&dev->drv[di]->rcv_waitq[channel]);
	} else
		dev_kfree_skb(skb);
}