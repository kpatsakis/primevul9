isdn_net_close(struct net_device *dev)
{
	struct net_device *p;
#ifdef CONFIG_ISDN_X25
	struct concap_proto *cprot =
		((isdn_net_local *)netdev_priv(dev))->netdev->cprot;
	/* printk(KERN_DEBUG "isdn_net_close %s\n" , dev-> name); */
#endif

#ifdef CONFIG_ISDN_X25
	if (cprot && cprot->pops) cprot->pops->close(cprot);
#endif
	netif_stop_queue(dev);
	p = MASTER_TO_SLAVE(dev);
	if (p) {
		/* If this interface has slaves, stop them also */
		while (p) {
#ifdef CONFIG_ISDN_X25
			cprot = ((isdn_net_local *)netdev_priv(p))
				->netdev->cprot;
			if (cprot && cprot->pops)
				cprot->pops->close(cprot);
#endif
			isdn_net_hangup(p);
			p = MASTER_TO_SLAVE(p);
		}
	}
	isdn_net_hangup(dev);
	isdn_unlock_drivers();
	return 0;
}