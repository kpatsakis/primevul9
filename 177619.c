isdn_unlock_drivers(void)
{
	int i;

	for (i = 0; i < ISDN_MAX_DRIVERS; i++) {
		if (!dev->drv[i])
			continue;
		isdn_unlock_driver(dev->drv[i]);
	}
}