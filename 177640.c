isdn_add_channels(isdn_driver_t *d, int drvidx, int n, int adding)
{
	int j, k, m;

	init_waitqueue_head(&d->st_waitq);
	if (d->flags & DRV_FLAG_RUNNING)
		return -1;
	if (n < 1) return 0;

	m = (adding) ? d->channels + n : n;

	if (dev->channels + n > ISDN_MAX_CHANNELS) {
		printk(KERN_WARNING "register_isdn: Max. %d channels supported\n",
		       ISDN_MAX_CHANNELS);
		return -1;
	}

	if ((adding) && (d->rcverr))
		kfree(d->rcverr);
	if (!(d->rcverr = kzalloc(sizeof(int) * m, GFP_ATOMIC))) {
		printk(KERN_WARNING "register_isdn: Could not alloc rcverr\n");
		return -1;
	}

	if ((adding) && (d->rcvcount))
		kfree(d->rcvcount);
	if (!(d->rcvcount = kzalloc(sizeof(int) * m, GFP_ATOMIC))) {
		printk(KERN_WARNING "register_isdn: Could not alloc rcvcount\n");
		if (!adding)
			kfree(d->rcverr);
		return -1;
	}

	if ((adding) && (d->rpqueue)) {
		for (j = 0; j < d->channels; j++)
			skb_queue_purge(&d->rpqueue[j]);
		kfree(d->rpqueue);
	}
	if (!(d->rpqueue = kmalloc(sizeof(struct sk_buff_head) * m, GFP_ATOMIC))) {
		printk(KERN_WARNING "register_isdn: Could not alloc rpqueue\n");
		if (!adding) {
			kfree(d->rcvcount);
			kfree(d->rcverr);
		}
		return -1;
	}
	for (j = 0; j < m; j++) {
		skb_queue_head_init(&d->rpqueue[j]);
	}

	if ((adding) && (d->rcv_waitq))
		kfree(d->rcv_waitq);
	d->rcv_waitq = kmalloc(sizeof(wait_queue_head_t) * 2 * m, GFP_ATOMIC);
	if (!d->rcv_waitq) {
		printk(KERN_WARNING "register_isdn: Could not alloc rcv_waitq\n");
		if (!adding) {
			kfree(d->rpqueue);
			kfree(d->rcvcount);
			kfree(d->rcverr);
		}
		return -1;
	}
	d->snd_waitq = d->rcv_waitq + m;
	for (j = 0; j < m; j++) {
		init_waitqueue_head(&d->rcv_waitq[j]);
		init_waitqueue_head(&d->snd_waitq[j]);
	}

	dev->channels += n;
	for (j = d->channels; j < m; j++)
		for (k = 0; k < ISDN_MAX_CHANNELS; k++)
			if (dev->chanmap[k] < 0) {
				dev->chanmap[k] = j;
				dev->drvmap[k] = drvidx;
				break;
			}
	d->channels = m;
	return 0;
}