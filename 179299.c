static int rtw_set_wps_beacon(struct net_device *dev, struct ieee_param *param, int len)
{
	unsigned char wps_oui[4] = {0x0, 0x50, 0xf2, 0x04};
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	int ie_len;

	DBG_88E("%s, len =%d\n", __func__, len);

	if (!check_fwstate(pmlmepriv, WIFI_AP_STATE))
		return -EINVAL;

	ie_len = len - 12 - 2; /* 12 = param header, 2:no packed */

	kfree(pmlmepriv->wps_beacon_ie);
	pmlmepriv->wps_beacon_ie = NULL;

	if (ie_len > 0) {
		pmlmepriv->wps_beacon_ie = rtw_malloc(ie_len);
		pmlmepriv->wps_beacon_ie_len = ie_len;
		if (!pmlmepriv->wps_beacon_ie) {
			DBG_88E("%s()-%d: rtw_malloc() ERROR!\n", __func__, __LINE__);
			return -EINVAL;
		}

		memcpy(pmlmepriv->wps_beacon_ie, param->u.bcn_ie.buf, ie_len);

		update_beacon(padapter, WLAN_EID_VENDOR_SPECIFIC, wps_oui, true);

		pmlmeext->bstart_bss = true;
	}

	return 0;
}