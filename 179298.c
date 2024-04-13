static int rtw_get_sta_wpaie(struct net_device *dev, struct ieee_param *param)
{
	int ret = 0;
	struct sta_info *psta = NULL;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct sta_priv *pstapriv = &padapter->stapriv;

	DBG_88E("%s, sta_addr: %pM\n", __func__, (param->sta_addr));

	if (!check_fwstate(pmlmepriv, _FW_LINKED | WIFI_AP_STATE))
		return -EINVAL;

	if (is_broadcast_ether_addr(param->sta_addr))
		return -EINVAL;

	psta = rtw_get_stainfo(pstapriv, param->sta_addr);
	if (psta) {
		if (psta->wpa_ie[0] == WLAN_EID_RSN ||
		    psta->wpa_ie[0] == WLAN_EID_VENDOR_SPECIFIC) {
			int wpa_ie_len;
			int copy_len;

			wpa_ie_len = psta->wpa_ie[1];
			copy_len = min_t(int, wpa_ie_len + 2, sizeof(psta->wpa_ie));
			param->u.wpa_ie.len = copy_len;
			memcpy(param->u.wpa_ie.reserved, psta->wpa_ie, copy_len);
		} else {
			DBG_88E("sta's wpa_ie is NONE\n");
		}
	} else {
		ret = -1;
	}

	return ret;
}