void indicate_wx_scan_complete_event(struct adapter *padapter)
{
	union iwreq_data wrqu;

	memset(&wrqu, 0, sizeof(union iwreq_data));
	wireless_send_event(padapter->pnetdev, SIOCGIWSCAN, &wrqu, NULL);
}