static int rtw_wx_set_wap(struct net_device *dev,
			  struct iw_request_info *info,
			  union iwreq_data *awrq, char *extra)
{
	uint ret = 0;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct sockaddr *temp = (struct sockaddr *)awrq;
	struct	mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct list_head *phead;
	u8 *dst_bssid, *src_bssid;
	struct __queue *queue	= &pmlmepriv->scanned_queue;
	struct	wlan_network	*pnetwork = NULL;
	enum ndis_802_11_auth_mode	authmode;

	if (!rtw_pwr_wakeup(padapter)) {
		ret = -1;
		goto exit;
	}

	if (!padapter->bup) {
		ret = -1;
		goto exit;
	}

	if (temp->sa_family != ARPHRD_ETHER) {
		ret = -EINVAL;
		goto exit;
	}

	authmode = padapter->securitypriv.ndisauthtype;
	spin_lock_bh(&queue->lock);
	phead = get_list_head(queue);
	pmlmepriv->pscanned = phead->next;

	while (phead != pmlmepriv->pscanned) {
		pnetwork = container_of(pmlmepriv->pscanned, struct wlan_network, list);

		pmlmepriv->pscanned = pmlmepriv->pscanned->next;

		dst_bssid = pnetwork->network.MacAddress;

		src_bssid = temp->sa_data;

		if ((!memcmp(dst_bssid, src_bssid, ETH_ALEN))) {
			if (!rtw_set_802_11_infrastructure_mode(padapter, pnetwork->network.InfrastructureMode)) {
				ret = -1;
				spin_unlock_bh(&queue->lock);
				goto exit;
			}

				break;
		}
	}
	spin_unlock_bh(&queue->lock);

	rtw_set_802_11_authentication_mode(padapter, authmode);
	if (!rtw_set_802_11_bssid(padapter, temp->sa_data)) {
		ret = -1;
		goto exit;
	}

exit:

	return ret;
}