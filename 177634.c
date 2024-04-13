register_isdn(isdn_if *i)
{
	isdn_driver_t *d;
	int j;
	ulong flags;
	int drvidx;

	if (dev->drivers >= ISDN_MAX_DRIVERS) {
		printk(KERN_WARNING "register_isdn: Max. %d drivers supported\n",
		       ISDN_MAX_DRIVERS);
		return 0;
	}
	if (!i->writebuf_skb) {
		printk(KERN_WARNING "register_isdn: No write routine given.\n");
		return 0;
	}
	if (!(d = kzalloc(sizeof(isdn_driver_t), GFP_KERNEL))) {
		printk(KERN_WARNING "register_isdn: Could not alloc driver-struct\n");
		return 0;
	}

	d->maxbufsize = i->maxbufsize;
	d->pktcount = 0;
	d->stavail = 0;
	d->flags = DRV_FLAG_LOADED;
	d->online = 0;
	d->interface = i;
	d->channels = 0;
	spin_lock_irqsave(&dev->lock, flags);
	for (drvidx = 0; drvidx < ISDN_MAX_DRIVERS; drvidx++)
		if (!dev->drv[drvidx])
			break;
	if (isdn_add_channels(d, drvidx, i->channels, 0)) {
		spin_unlock_irqrestore(&dev->lock, flags);
		kfree(d);
		return 0;
	}
	i->channels = drvidx;
	i->rcvcallb_skb = isdn_receive_skb_callback;
	i->statcallb = isdn_status_callback;
	if (!strlen(i->id))
		sprintf(i->id, "line%d", drvidx);
	for (j = 0; j < drvidx; j++)
		if (!strcmp(i->id, dev->drvid[j]))
			sprintf(i->id, "line%d", drvidx);
	dev->drv[drvidx] = d;
	strcpy(dev->drvid[drvidx], i->id);
	isdn_info_update();
	dev->drivers++;
	set_global_features();
	spin_unlock_irqrestore(&dev->lock, flags);
	return 1;
}