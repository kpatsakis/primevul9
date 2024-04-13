static int rtw_del_sta(struct net_device *dev, struct ieee_param *param)
{
	struct sta_info *psta = NULL;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	int updated = 0;

	DBG_88E("%s =%pM\n", __func__, (param->sta_addr));

	if (!check_fwstate(pmlmepriv, _FW_LINKED | WIFI_AP_STATE))
		return -EINVAL;

	if (is_broadcast_ether_addr(param->sta_addr))
		return -EINVAL;

	psta = rtw_get_stainfo(pstapriv, param->sta_addr);
	if (psta) {
		spin_lock_bh(&pstapriv->asoc_list_lock);
		if (!list_empty(&psta->asoc_list)) {
			list_del_init(&psta->asoc_list);
			pstapriv->asoc_list_cnt--;
			updated = ap_free_sta(padapter, psta, true, WLAN_REASON_DEAUTH_LEAVING);
		}
		spin_unlock_bh(&pstapriv->asoc_list_lock);
		associated_clients_update(padapter, updated);
		psta = NULL;
	} else {
		DBG_88E("%s(), sta has already been removed or never been added\n", __func__);
	}

	return 0;
}