static int rtw_wx_get_scan(struct net_device *dev, struct iw_request_info *a,
			   union iwreq_data *wrqu, char *extra)
{
	struct list_head *plist, *phead;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct	mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct __queue *queue	= &pmlmepriv->scanned_queue;
	struct	wlan_network	*pnetwork = NULL;
	char *ev = extra;
	char *stop = ev + wrqu->data.length;
	u32 ret = 0;
	u32 cnt = 0;
	u32 wait_for_surveydone;
	int wait_status;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("%s\n", __func__));
	RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_, (" Start of Query SIOCGIWSCAN .\n"));

	if (padapter->pwrctrlpriv.brfoffbyhw && padapter->bDriverStopped) {
		ret = -EINVAL;
		goto exit;
	}

	wait_for_surveydone = 100;

	wait_status = _FW_UNDER_SURVEY | _FW_UNDER_LINKING;

	while (check_fwstate(pmlmepriv, wait_status)) {
		msleep(30);
		cnt++;
		if (cnt > wait_for_surveydone)
			break;
	}

	spin_lock_bh(&pmlmepriv->scanned_queue.lock);

	phead = get_list_head(queue);
	plist = phead->next;

	while (phead != plist) {
		if ((stop - ev) < SCAN_ITEM_SIZE) {
			ret = -E2BIG;
			break;
		}

		pnetwork = container_of(plist, struct wlan_network, list);

		/* report network only if the current channel set contains the channel to which this network belongs */
		if (rtw_ch_set_search_ch(padapter->mlmeextpriv.channel_set, pnetwork->network.Configuration.DSConfig) >= 0)
			ev = translate_scan(padapter, a, pnetwork, ev, stop);

		plist = plist->next;
	}

	spin_unlock_bh(&pmlmepriv->scanned_queue.lock);

	wrqu->data.length = ev - extra;
	wrqu->data.flags = 0;

exit:
	return ret;
}