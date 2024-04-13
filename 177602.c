isdn_get_free_channel(int usage, int l2_proto, int l3_proto, int pre_dev
		      , int pre_chan, char *msn)
{
	int i;
	ulong features;
	ulong vfeatures;

	features = ((1 << l2_proto) | (0x10000 << l3_proto));
	vfeatures = (((1 << l2_proto) | (0x10000 << l3_proto)) &
		     ~(ISDN_FEATURE_L2_V11096 | ISDN_FEATURE_L2_V11019 | ISDN_FEATURE_L2_V11038));
	/* If Layer-2 protocol is V.110, accept drivers with
	 * transparent feature even if these don't support V.110
	 * because we can emulate this in linklevel.
	 */
	for (i = 0; i < ISDN_MAX_CHANNELS; i++)
		if (USG_NONE(dev->usage[i]) &&
		    (dev->drvmap[i] != -1)) {
			int d = dev->drvmap[i];
			if ((dev->usage[i] & ISDN_USAGE_EXCLUSIVE) &&
			    ((pre_dev != d) || (pre_chan != dev->chanmap[i])))
				continue;
			if (!strcmp(isdn_map_eaz2msn(msn, d), "-"))
				continue;
			if (dev->usage[i] & ISDN_USAGE_DISABLED)
				continue; /* usage not allowed */
			if (dev->drv[d]->flags & DRV_FLAG_RUNNING) {
				if (((dev->drv[d]->interface->features & features) == features) ||
				    (((dev->drv[d]->interface->features & vfeatures) == vfeatures) &&
				     (dev->drv[d]->interface->features & ISDN_FEATURE_L2_TRANS))) {
					if ((pre_dev < 0) || (pre_chan < 0)) {
						dev->usage[i] &= ISDN_USAGE_EXCLUSIVE;
						dev->usage[i] |= usage;
						isdn_info_update();
						return i;
					} else {
						if ((pre_dev == d) && (pre_chan == dev->chanmap[i])) {
							dev->usage[i] &= ISDN_USAGE_EXCLUSIVE;
							dev->usage[i] |= usage;
							isdn_info_update();
							return i;
						}
					}
				}
			}
		}
	return -1;
}