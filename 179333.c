static int rtw_add_sta(struct net_device *dev, struct ieee_param *param)
{
	int ret = 0;
	struct sta_info *psta = NULL;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct sta_priv *pstapriv = &padapter->stapriv;

	DBG_88E("%s(aid =%d) =%pM\n", __func__, param->u.add_sta.aid, (param->sta_addr));

	if (!check_fwstate(pmlmepriv, (_FW_LINKED | WIFI_AP_STATE)))
		return -EINVAL;

	if (is_broadcast_ether_addr(param->sta_addr))
		return -EINVAL;

	psta = rtw_get_stainfo(pstapriv, param->sta_addr);
	if (psta) {
		int flags = param->u.add_sta.flags;

		psta->aid = param->u.add_sta.aid;/* aid = 1~2007 */

		memcpy(psta->bssrateset, param->u.add_sta.tx_supp_rates, 16);

		/* check wmm cap. */
		if (WLAN_STA_WME & flags)
			psta->qos_option = 1;
		else
			psta->qos_option = 0;

		if (pmlmepriv->qospriv.qos_option == 0)
			psta->qos_option = 0;

		/* chec 802.11n ht cap. */
		if (WLAN_STA_HT & flags) {
			psta->htpriv.ht_option = true;
			psta->qos_option = 1;
			memcpy(&psta->htpriv.ht_cap, &param->u.add_sta.ht_cap,
			       sizeof(struct ieee80211_ht_cap));
		} else {
			psta->htpriv.ht_option = false;
		}

		if (!pmlmepriv->htpriv.ht_option)
			psta->htpriv.ht_option = false;

		update_sta_info_apmode(padapter, psta);
	} else {
		ret = -ENOMEM;
	}

	return ret;
}