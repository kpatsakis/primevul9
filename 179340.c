static int rtw_wx_get_sens(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	wrqu->sens.value = 0;
	wrqu->sens.fixed = 0;	/* no auto select */
	wrqu->sens.disabled = 1;
	return 0;
}