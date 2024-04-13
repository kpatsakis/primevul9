isdn_net_getcfg(isdn_net_ioctl_cfg *cfg)
{
	isdn_net_dev *p = isdn_net_findif(cfg->name);

	if (p) {
		isdn_net_local *lp = p->local;

		strcpy(cfg->eaz, lp->msn);
		cfg->exclusive = lp->exclusive;
		if (lp->pre_device >= 0) {
			sprintf(cfg->drvid, "%s,%d", dev->drvid[lp->pre_device],
				lp->pre_channel);
		} else
			cfg->drvid[0] = '\0';
		cfg->onhtime = lp->onhtime;
		cfg->charge = lp->charge;
		cfg->l2_proto = lp->l2_proto;
		cfg->l3_proto = lp->l3_proto;
		cfg->p_encap = lp->p_encap;
		cfg->secure = (lp->flags & ISDN_NET_SECURE) ? 1 : 0;
		cfg->callback = 0;
		if (lp->flags & ISDN_NET_CALLBACK)
			cfg->callback = 1;
		if (lp->flags & ISDN_NET_CBOUT)
			cfg->callback = 2;
		cfg->cbhup = (lp->flags & ISDN_NET_CBHUP) ? 1 : 0;
		cfg->dialmode = lp->flags & ISDN_NET_DIALMODE_MASK;
		cfg->chargehup = (lp->hupflags & ISDN_CHARGEHUP) ? 1 : 0;
		cfg->ihup = (lp->hupflags & ISDN_INHUP) ? 1 : 0;
		cfg->cbdelay = lp->cbdelay;
		cfg->dialmax = lp->dialmax;
		cfg->triggercps = lp->triggercps;
		cfg->slavedelay = lp->slavedelay / HZ;
		cfg->chargeint = (lp->hupflags & ISDN_CHARGEHUP) ?
			(lp->chargeint / HZ) : 0;
		cfg->pppbind = lp->pppbind;
		cfg->dialtimeout = lp->dialtimeout >= 0 ? lp->dialtimeout / HZ : -1;
		cfg->dialwait = lp->dialwait / HZ;
		if (lp->slave) {
			if (strlen(lp->slave->name) >= 10)
				strcpy(cfg->slave, "too-long");
			else
				strcpy(cfg->slave, lp->slave->name);
		} else
			cfg->slave[0] = '\0';
		if (lp->master) {
			if (strlen(lp->master->name) >= 10)
				strcpy(cfg->master, "too-long");
			else
				strcpy(cfg->master, lp->master->name);
		} else
			cfg->master[0] = '\0';
		return 0;
	}
	return -ENODEV;
}