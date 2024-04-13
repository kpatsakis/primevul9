isdn_writebuf_stub(int drvidx, int chan, const u_char __user *buf, int len)
{
	int ret;
	int hl = dev->drv[drvidx]->interface->hl_hdrlen;
	struct sk_buff *skb = alloc_skb(hl + len, GFP_ATOMIC);

	if (!skb)
		return -ENOMEM;
	skb_reserve(skb, hl);
	if (copy_from_user(skb_put(skb, len), buf, len)) {
		dev_kfree_skb(skb);
		return -EFAULT;
	}
	ret = dev->drv[drvidx]->interface->writebuf_skb(drvidx, chan, 1, skb);
	if (ret <= 0)
		dev_kfree_skb(skb);
	if (ret > 0)
		dev->obytes[isdn_dc2minor(drvidx, chan)] += ret;
	return ret;
}