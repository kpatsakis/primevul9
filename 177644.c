isdn_ciscohdlck_dev_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	isdn_net_local *lp = netdev_priv(dev);
	unsigned long len = 0;
	unsigned long expires = 0;
	int tmp = 0;
	int period = lp->cisco_keepalive_period;
	s8 debserint = lp->cisco_debserint;
	int rc = 0;

	if (lp->p_encap != ISDN_NET_ENCAP_CISCOHDLCK)
		return -EINVAL;

	switch (cmd) {
		/* get/set keepalive period */
	case SIOCGKEEPPERIOD:
		len = (unsigned long)sizeof(lp->cisco_keepalive_period);
		if (copy_to_user(ifr->ifr_data,
				 &lp->cisco_keepalive_period, len))
			rc = -EFAULT;
		break;
	case SIOCSKEEPPERIOD:
		tmp = lp->cisco_keepalive_period;
		len = (unsigned long)sizeof(lp->cisco_keepalive_period);
		if (copy_from_user(&period, ifr->ifr_data, len))
			rc = -EFAULT;
		if ((period > 0) && (period <= 32767))
			lp->cisco_keepalive_period = period;
		else
			rc = -EINVAL;
		if (!rc && (tmp != lp->cisco_keepalive_period)) {
			expires = (unsigned long)(jiffies +
						  lp->cisco_keepalive_period * HZ);
			mod_timer(&lp->cisco_timer, expires);
			printk(KERN_INFO "%s: Keepalive period set "
			       "to %d seconds.\n",
			       dev->name, lp->cisco_keepalive_period);
		}
		break;

		/* get/set debugging */
	case SIOCGDEBSERINT:
		len = (unsigned long)sizeof(lp->cisco_debserint);
		if (copy_to_user(ifr->ifr_data,
				 &lp->cisco_debserint, len))
			rc = -EFAULT;
		break;
	case SIOCSDEBSERINT:
		len = (unsigned long)sizeof(lp->cisco_debserint);
		if (copy_from_user(&debserint,
				   ifr->ifr_data, len))
			rc = -EFAULT;
		if ((debserint >= 0) && (debserint <= 64))
			lp->cisco_debserint = debserint;
		else
			rc = -EINVAL;
		break;

	default:
		rc = -EINVAL;
		break;
	}
	return (rc);
}