static int rtw_wx_set_rts(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	struct adapter *padapter = rtw_netdev_priv(dev);

	if (wrqu->rts.disabled) {
		padapter->registrypriv.rts_thresh = 2347;
	} else {
		if (wrqu->rts.value < 0 ||
		    wrqu->rts.value > 2347)
			return -EINVAL;

		padapter->registrypriv.rts_thresh = wrqu->rts.value;
	}

	DBG_88E("%s, rts_thresh =%d\n", __func__, padapter->registrypriv.rts_thresh);

	return 0;
}