static int rtw_wx_set_essid(struct net_device *dev,
			    struct iw_request_info *a,
			    union iwreq_data *wrqu, char *extra)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct __queue *queue = &pmlmepriv->scanned_queue;
	struct list_head *phead;
	struct wlan_network *pnetwork = NULL;
	enum ndis_802_11_auth_mode authmode;
	struct ndis_802_11_ssid ndis_ssid;
	u8 *dst_ssid, *src_ssid;

	uint ret = 0, len;

	RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_,
		 ("+%s: fw_state = 0x%08x\n", __func__, get_fwstate(pmlmepriv)));
	if (!rtw_pwr_wakeup(padapter)) {
		ret = -1;
		goto exit;
	}

	if (!padapter->bup) {
		ret = -1;
		goto exit;
	}

	if (wrqu->essid.length > IW_ESSID_MAX_SIZE) {
		ret = -E2BIG;
		goto exit;
	}

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		ret = -1;
		goto exit;
	}

	authmode = padapter->securitypriv.ndisauthtype;
	DBG_88E("=>%s\n", __func__);
	if (wrqu->essid.flags && wrqu->essid.length) {
		len = min_t(uint, wrqu->essid.length, IW_ESSID_MAX_SIZE);

		if (wrqu->essid.length != 33)
			DBG_88E("ssid =%s, len =%d\n", extra, wrqu->essid.length);

		memset(&ndis_ssid, 0, sizeof(struct ndis_802_11_ssid));
		ndis_ssid.ssid_length = len;
		memcpy(ndis_ssid.ssid, extra, len);
		src_ssid = ndis_ssid.ssid;

		RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_, ("%s: ssid =[%s]\n", __func__, src_ssid));
		spin_lock_bh(&queue->lock);
		phead = get_list_head(queue);
		pmlmepriv->pscanned = phead->next;

		while (phead != pmlmepriv->pscanned) {
			pnetwork = container_of(pmlmepriv->pscanned, struct wlan_network, list);

			pmlmepriv->pscanned = pmlmepriv->pscanned->next;

			dst_ssid = pnetwork->network.ssid.ssid;

			RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_,
				 ("%s: dst_ssid =%s\n", __func__,
				  pnetwork->network.ssid.ssid));

			if ((!memcmp(dst_ssid, src_ssid, ndis_ssid.ssid_length)) &&
			    (pnetwork->network.ssid.ssid_length == ndis_ssid.ssid_length)) {
				RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_,
					 ("%s: find match, set infra mode\n", __func__));

				if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)) {
					if (pnetwork->network.InfrastructureMode != pmlmepriv->cur_network.network.InfrastructureMode)
						continue;
				}

				if (!rtw_set_802_11_infrastructure_mode(padapter, pnetwork->network.InfrastructureMode)) {
					ret = -1;
					spin_unlock_bh(&queue->lock);
					goto exit;
				}

				break;
			}
		}
		spin_unlock_bh(&queue->lock);
		RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_,
			 ("set ssid: set_802_11_auth. mode =%d\n", authmode));
		rtw_set_802_11_authentication_mode(padapter, authmode);
		if (!rtw_set_802_11_ssid(padapter, &ndis_ssid)) {
			ret = -1;
			goto exit;
		}
	}

exit:
	DBG_88E("<=%s, ret %d\n", __func__, ret);

	return ret;
}