static int rtw_wx_set_mlme(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
	u16 reason;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct iw_mlme *mlme = (struct iw_mlme *)extra;

	if (!mlme)
		return -1;

	DBG_88E("%s\n", __func__);

	reason = mlme->reason_code;

	DBG_88E("%s, cmd =%d, reason =%d\n", __func__, mlme->cmd, reason);

	switch (mlme->cmd) {
	case IW_MLME_DEAUTH:
		if (!rtw_set_802_11_disassociate(padapter))
			ret = -1;
		break;
	case IW_MLME_DISASSOC:
		if (!rtw_set_802_11_disassociate(padapter))
			ret = -1;
		break;
	default:
		return -EOPNOTSUPP;
	}
	return ret;
}