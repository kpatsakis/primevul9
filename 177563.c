isdn_net_swap_usage(int i1, int i2)
{
	int u1 = dev->usage[i1] & ISDN_USAGE_EXCLUSIVE;
	int u2 = dev->usage[i2] & ISDN_USAGE_EXCLUSIVE;

#ifdef ISDN_DEBUG_NET_ICALL
	printk(KERN_DEBUG "n_fi: usage of %d and %d\n", i1, i2);
#endif
	dev->usage[i1] &= ~ISDN_USAGE_EXCLUSIVE;
	dev->usage[i1] |= u2;
	dev->usage[i2] &= ~ISDN_USAGE_EXCLUSIVE;
	dev->usage[i2] |= u1;
	isdn_info_update();
}