static int rtw_wx_set_priv(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *awrq, char *extra)
{
	int ret = 0;
	int len = 0;
	char *ext;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct iw_point *dwrq = (struct iw_point *)awrq;

	if (dwrq->length == 0)
		return -EFAULT;

	len = dwrq->length;
	ext = vmalloc(len);
	if (!ext)
		return -ENOMEM;

	if (copy_from_user(ext, dwrq->pointer, len)) {
		vfree(ext);
		return -EFAULT;
	}

	/* added for wps2.0 @20110524 */
	if (dwrq->flags == 0x8766 && len > 8) {
		u32 cp_sz;
		struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
		u8 *probereq_wpsie = ext;
		int probereq_wpsie_len = len;
		u8 wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};

		if ((probereq_wpsie[0] == WLAN_EID_VENDOR_SPECIFIC) &&
		    (!memcmp(&probereq_wpsie[2], wps_oui, 4))) {
			cp_sz = min(probereq_wpsie_len, MAX_WPS_IE_LEN);

			pmlmepriv->wps_probe_req_ie_len = 0;
			kfree(pmlmepriv->wps_probe_req_ie);
			pmlmepriv->wps_probe_req_ie = NULL;

			pmlmepriv->wps_probe_req_ie = rtw_malloc(cp_sz);
			if (!pmlmepriv->wps_probe_req_ie) {
				pr_info("%s()-%d: rtw_malloc() ERROR!\n", __func__, __LINE__);
				ret =  -EINVAL;
				goto FREE_EXT;
			}
			memcpy(pmlmepriv->wps_probe_req_ie, probereq_wpsie, cp_sz);
			pmlmepriv->wps_probe_req_ie_len = cp_sz;
		}
		goto FREE_EXT;
	}

	if (len >= WEXT_CSCAN_HEADER_SIZE &&
	    !memcmp(ext, WEXT_CSCAN_HEADER, WEXT_CSCAN_HEADER_SIZE)) {
		ret = rtw_wx_set_scan(dev, info, awrq, ext);
		goto FREE_EXT;
	}

FREE_EXT:

	vfree(ext);

	return ret;
}