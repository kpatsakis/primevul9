isdn_free_channel(int di, int ch, int usage)
{
	int i;

	if ((di < 0) || (ch < 0)) {
		printk(KERN_WARNING "%s: called with invalid drv(%d) or channel(%d)\n",
		       __func__, di, ch);
		return;
	}
	for (i = 0; i < ISDN_MAX_CHANNELS; i++)
		if (((!usage) || ((dev->usage[i] & ISDN_USAGE_MASK) == usage)) &&
		    (dev->drvmap[i] == di) &&
		    (dev->chanmap[i] == ch)) {
			dev->usage[i] &= (ISDN_USAGE_NONE | ISDN_USAGE_EXCLUSIVE);
			strcpy(dev->num[i], "???");
			dev->ibytes[i] = 0;
			dev->obytes[i] = 0;
// 20.10.99 JIM, try to reinitialize v110 !
			dev->v110emu[i] = 0;
			atomic_set(&(dev->v110use[i]), 0);
			isdn_v110_close(dev->v110[i]);
			dev->v110[i] = NULL;
// 20.10.99 JIM, try to reinitialize v110 !
			isdn_info_update();
			if (dev->drv[di])
				skb_queue_purge(&dev->drv[di]->rpqueue[ch]);
		}
}