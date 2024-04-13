static int check_device_status(struct xc2028_data *priv)
{
	switch (priv->state) {
	case XC2028_NO_FIRMWARE:
	case XC2028_WAITING_FIRMWARE:
		return -EAGAIN;
	case XC2028_ACTIVE:
		return 1;
	case XC2028_SLEEP:
		return 0;
	case XC2028_NODEV:
		return -ENODEV;
	}
	return 0;
}