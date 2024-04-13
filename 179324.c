static int rtw_wx_set_gen_ie(struct net_device *dev,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	struct adapter *padapter = rtw_netdev_priv(dev);

	return rtw_set_wpa_ie(padapter, extra, wrqu->data.length);
}