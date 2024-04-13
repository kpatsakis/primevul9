void rtw_indicate_wx_assoc_event(struct adapter *padapter)
{
	union iwreq_data wrqu;
	struct	mlme_priv *pmlmepriv = &padapter->mlmepriv;

	memset(&wrqu, 0, sizeof(union iwreq_data));

	wrqu.ap_addr.sa_family = ARPHRD_ETHER;

	memcpy(wrqu.ap_addr.sa_data, pmlmepriv->cur_network.network.MacAddress, ETH_ALEN);

	DBG_88E_LEVEL(_drv_always_, "assoc success\n");
	wireless_send_event(padapter->pnetdev, SIOCGIWAP, &wrqu, NULL);
}