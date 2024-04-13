static int rtw_wx_get_retry(struct net_device *dev,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	wrqu->retry.value = 7;
	wrqu->retry.fixed = 0;	/* no auto select */
	wrqu->retry.disabled = 1;

	return 0;
}