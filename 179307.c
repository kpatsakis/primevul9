static int rtw_set_hidden_ssid(struct net_device *dev, struct ieee_param *param, int len)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;

	u8 value;

	if (!check_fwstate(pmlmepriv, WIFI_AP_STATE))
		return -EINVAL;

	if (param->u.wpa_param.name != 0) /* dummy test... */
		DBG_88E("%s name(%u) != 0\n", __func__, param->u.wpa_param.name);
	value = param->u.wpa_param.value;

	/* use the same definition of hostapd's ignore_broadcast_ssid */
	if (value != 1 && value != 2)
		value = 0;
	DBG_88E("%s value(%u)\n", __func__, value);
	pmlmeinfo->hidden_ssid_mode = value;
	return 0;
}