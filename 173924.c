static int sony_get_bt_devaddr(struct sony_sc *sc)
{
	int ret;

	/* HIDP stores the device MAC address as a string in the uniq field. */
	ret = strlen(sc->hdev->uniq);
	if (ret != 17)
		return -EINVAL;

	ret = sscanf(sc->hdev->uniq,
		"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		&sc->mac_address[5], &sc->mac_address[4], &sc->mac_address[3],
		&sc->mac_address[2], &sc->mac_address[1], &sc->mac_address[0]);

	if (ret != 6)
		return -EINVAL;

	return 0;
}