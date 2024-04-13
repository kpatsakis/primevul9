isdn_net_getpeer(isdn_net_ioctl_phone *phone, isdn_net_ioctl_phone __user *peer)
{
	isdn_net_dev *p = isdn_net_findif(phone->name);
	int ch, dv, idx;

	if (!p)
		return -ENODEV;
	/*
	 * Theoretical race: while this executes, the remote number might
	 * become invalid (hang up) or change (new connection), resulting
	 * in (partially) wrong number copied to user. This race
	 * currently ignored.
	 */
	ch = p->local->isdn_channel;
	dv = p->local->isdn_device;
	if (ch < 0 && dv < 0)
		return -ENOTCONN;
	idx = isdn_dc2minor(dv, ch);
	if (idx < 0)
		return -ENODEV;
	/* for pre-bound channels, we need this extra check */
	if (strncmp(dev->num[idx], "???", 3) == 0)
		return -ENOTCONN;
	strncpy(phone->phone, dev->num[idx], ISDN_MSNLEN);
	phone->outgoing = USG_OUTGOING(dev->usage[idx]);
	if (copy_to_user(peer, phone, sizeof(*peer)))
		return -EFAULT;
	return 0;
}