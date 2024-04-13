static int rtw_wx_get_power(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	wrqu->power.value = 0;
	wrqu->power.fixed = 0;	/* no auto select */
	wrqu->power.disabled = 1;

	return 0;
}