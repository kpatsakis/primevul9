set_global_features(void)
{
	int drvidx;

	dev->global_features = 0;
	for (drvidx = 0; drvidx < ISDN_MAX_DRIVERS; drvidx++) {
		if (!dev->drv[drvidx])
			continue;
		if (dev->drv[drvidx]->interface)
			dev->global_features |= dev->drv[drvidx]->interface->features;
	}
}