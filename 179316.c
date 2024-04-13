static int wpa_mlme(struct net_device *dev, u32 command, u32 reason)
{
	int ret = 0;
	struct adapter *padapter = rtw_netdev_priv(dev);

	switch (command) {
	case IEEE_MLME_STA_DEAUTH:
		if (!rtw_set_802_11_disassociate(padapter))
			ret = -1;
		break;
	case IEEE_MLME_STA_DISASSOC:
		if (!rtw_set_802_11_disassociate(padapter))
			ret = -1;
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	return ret;
}