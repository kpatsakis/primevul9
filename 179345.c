static int rtw_wx_set_frag(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	struct adapter *padapter = rtw_netdev_priv(dev);

	if (wrqu->frag.disabled) {
		padapter->xmitpriv.frag_len = MAX_FRAG_THRESHOLD;
	} else {
		if (wrqu->frag.value < MIN_FRAG_THRESHOLD ||
		    wrqu->frag.value > MAX_FRAG_THRESHOLD)
			return -EINVAL;

		padapter->xmitpriv.frag_len = wrqu->frag.value & ~0x1;
	}

	DBG_88E("%s, frag_len =%d\n", __func__, padapter->xmitpriv.frag_len);

	return 0;
}