static int rtw_wx_get_frag(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	struct adapter *padapter = rtw_netdev_priv(dev);

	DBG_88E("%s, frag_len =%d\n", __func__, padapter->xmitpriv.frag_len);

	wrqu->frag.value = padapter->xmitpriv.frag_len;
	wrqu->frag.fixed = 0;	/* no auto select */

	return 0;
}