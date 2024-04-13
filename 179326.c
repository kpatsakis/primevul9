int rtw_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct iwreq *wrq = (struct iwreq *)rq;
	int ret = 0;

	switch (cmd) {
	case RTL_IOCTL_WPA_SUPPLICANT:
		ret = wpa_supplicant_ioctl(dev, &wrq->u.data);
		break;
#ifdef CONFIG_88EU_AP_MODE
	case RTL_IOCTL_HOSTAPD:
		ret = rtw_hostapd_ioctl(dev, &wrq->u.data);
		break;
#endif /*  CONFIG_88EU_AP_MODE */
	case (SIOCDEVPRIVATE + 1):
		ret = rtw_android_priv_cmd(dev, rq, cmd);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}
	return ret;
}