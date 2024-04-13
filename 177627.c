isdn_net_hangup(struct net_device *d)
{
	isdn_net_local *lp = netdev_priv(d);
	isdn_ctrl cmd;
#ifdef CONFIG_ISDN_X25
	struct concap_proto *cprot = lp->netdev->cprot;
	struct concap_proto_ops *pops = cprot ? cprot->pops : NULL;
#endif

	if (lp->flags & ISDN_NET_CONNECTED) {
		if (lp->slave != NULL) {
			isdn_net_local *slp = ISDN_SLAVE_PRIV(lp);
			if (slp->flags & ISDN_NET_CONNECTED) {
				printk(KERN_INFO
				       "isdn_net: hang up slave %s before %s\n",
				       lp->slave->name, d->name);
				isdn_net_hangup(lp->slave);
			}
		}
		printk(KERN_INFO "isdn_net: local hangup %s\n", d->name);
#ifdef CONFIG_ISDN_PPP
		if (lp->p_encap == ISDN_NET_ENCAP_SYNCPPP)
			isdn_ppp_free(lp);
#endif
		isdn_net_lp_disconnected(lp);
#ifdef CONFIG_ISDN_X25
		/* try if there are generic encap protocol
		   receiver routines and signal the closure of
		   the link */
		if (pops && pops->disconn_ind)
			pops->disconn_ind(cprot);
#endif /* CONFIG_ISDN_X25 */

		cmd.driver = lp->isdn_device;
		cmd.command = ISDN_CMD_HANGUP;
		cmd.arg = lp->isdn_channel;
		isdn_command(&cmd);
		printk(KERN_INFO "%s: Chargesum is %d\n", d->name, lp->charge);
		isdn_all_eaz(lp->isdn_device, lp->isdn_channel);
	}
	isdn_net_unbind_channel(lp);
}