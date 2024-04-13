isdn_net_autohup(void)
{
	isdn_net_dev *p = dev->netdev;
	int anymore;

	anymore = 0;
	while (p) {
		isdn_net_local *l = p->local;
		if (jiffies == last_jiffies)
			l->cps = l->transcount;
		else
			l->cps = (l->transcount * HZ) / (jiffies - last_jiffies);
		l->transcount = 0;
		if (dev->net_verbose > 3)
			printk(KERN_DEBUG "%s: %d bogocps\n", p->dev->name, l->cps);
		if ((l->flags & ISDN_NET_CONNECTED) && (!l->dialstate)) {
			anymore = 1;
			l->huptimer++;
			/*
			 * if there is some dialmode where timeout-hangup
			 * should _not_ be done, check for that here
			 */
			if ((l->onhtime) &&
			    (l->huptimer > l->onhtime))
			{
				if (l->hupflags & ISDN_MANCHARGE &&
				    l->hupflags & ISDN_CHARGEHUP) {
					while (time_after(jiffies, l->chargetime + l->chargeint))
						l->chargetime += l->chargeint;
					if (time_after(jiffies, l->chargetime + l->chargeint - 2 * HZ))
						if (l->outgoing || l->hupflags & ISDN_INHUP)
							isdn_net_hangup(p->dev);
				} else if (l->outgoing) {
					if (l->hupflags & ISDN_CHARGEHUP) {
						if (l->hupflags & ISDN_WAITCHARGE) {
							printk(KERN_DEBUG "isdn_net: Hupflags of %s are %X\n",
							       p->dev->name, l->hupflags);
							isdn_net_hangup(p->dev);
						} else if (time_after(jiffies, l->chargetime + l->chargeint)) {
							printk(KERN_DEBUG
							       "isdn_net: %s: chtime = %lu, chint = %d\n",
							       p->dev->name, l->chargetime, l->chargeint);
							isdn_net_hangup(p->dev);
						}
					} else
						isdn_net_hangup(p->dev);
				} else if (l->hupflags & ISDN_INHUP)
					isdn_net_hangup(p->dev);
			}

			if (dev->global_flags & ISDN_GLOBAL_STOPPED || (ISDN_NET_DIALMODE(*l) == ISDN_NET_DM_OFF)) {
				isdn_net_hangup(p->dev);
				break;
			}
		}
		p = (isdn_net_dev *) p->next;
	}
	last_jiffies = jiffies;
	isdn_timer_ctrl(ISDN_TIMER_NETHANGUP, anymore);
}