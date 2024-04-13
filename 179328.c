static int rtw_wx_get_wap(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct	mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct wlan_bssid_ex  *pcur_bss = &pmlmepriv->cur_network.network;

	wrqu->ap_addr.sa_family = ARPHRD_ETHER;

	eth_zero_addr(wrqu->ap_addr.sa_data);

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("%s\n", __func__));

	if (check_fwstate(pmlmepriv, _FW_LINKED) ||
	    check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) ||
	    check_fwstate(pmlmepriv, WIFI_AP_STATE))
		memcpy(wrqu->ap_addr.sa_data, pcur_bss->MacAddress, ETH_ALEN);
	else
		eth_zero_addr(wrqu->ap_addr.sa_data);
	return 0;
}