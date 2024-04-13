static int rtw_set_wps_probe_resp(struct net_device *dev, struct ieee_param *param, int len)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	int ie_len;

	DBG_88E("%s, len =%d\n", __func__, len);

	if (!check_fwstate(pmlmepriv, WIFI_AP_STATE))
		return -EINVAL;

	ie_len = len - 12 - 2; /* 12 = param header, 2:no packed */

	kfree(pmlmepriv->wps_probe_resp_ie);
	pmlmepriv->wps_probe_resp_ie = NULL;

	if (ie_len > 0) {
		pmlmepriv->wps_probe_resp_ie = rtw_malloc(ie_len);
		pmlmepriv->wps_probe_resp_ie_len = ie_len;
		if (!pmlmepriv->wps_probe_resp_ie) {
			DBG_88E("%s()-%d: rtw_malloc() ERROR!\n", __func__, __LINE__);
			return -EINVAL;
		}
		memcpy(pmlmepriv->wps_probe_resp_ie, param->u.bcn_ie.buf, ie_len);
	}

	return 0;
}