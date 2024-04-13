void rtw_indicate_wx_disassoc_event(struct adapter *padapter)
{
	union iwreq_data wrqu;

	memset(&wrqu, 0, sizeof(union iwreq_data));

	wrqu.ap_addr.sa_family = ARPHRD_ETHER;
	eth_zero_addr(wrqu.ap_addr.sa_data);

	DBG_88E_LEVEL(_drv_always_, "indicate disassoc\n");
	wireless_send_event(padapter->pnetdev, SIOCGIWAP, &wrqu, NULL);
}