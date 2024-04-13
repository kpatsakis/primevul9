static int rtw_wx_get_essid(struct net_device *dev,
			    struct iw_request_info *a,
			    union iwreq_data *wrqu, char *extra)
{
	u32 len;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct	mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct wlan_bssid_ex  *pcur_bss = &pmlmepriv->cur_network.network;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("%s\n", __func__));

	if ((check_fwstate(pmlmepriv, _FW_LINKED)) ||
	    (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE))) {
		len = pcur_bss->ssid.ssid_length;
		memcpy(extra, pcur_bss->ssid.ssid, len);
	} else {
		len = 0;
		*extra = 0;
	}
	wrqu->essid.length = len;
	wrqu->essid.flags = 1;

	return 0;
}