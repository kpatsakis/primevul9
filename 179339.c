static int rtw_wx_get_name(struct net_device *dev,
			   struct iw_request_info *info,
			   union iwreq_data *wrqu, char *extra)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	u32 ht_ielen = 0;
	char *p;
	u8 ht_cap = false;
	struct	mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct wlan_bssid_ex  *pcur_bss = &pmlmepriv->cur_network.network;
	NDIS_802_11_RATES_EX *prates = NULL;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("cmd_code =%x\n", info->cmd));

	if (check_fwstate(pmlmepriv, _FW_LINKED | WIFI_ADHOC_MASTER_STATE)) {
		/* parsing HT_CAP_IE */
		p = rtw_get_ie(&pcur_bss->ies[12], WLAN_EID_HT_CAPABILITY, &ht_ielen, pcur_bss->ie_length - 12);
		if (p && ht_ielen > 0)
			ht_cap = true;

		prates = &pcur_bss->SupportedRates;

		if (rtw_is_cckratesonly_included((u8 *)prates)) {
			if (ht_cap)
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bn");
			else
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11b");
		} else if (rtw_is_cckrates_included((u8 *)prates)) {
			if (ht_cap)
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bgn");
			else
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11bg");
		} else {
			if (ht_cap)
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11gn");
			else
				snprintf(wrqu->name, IFNAMSIZ, "IEEE 802.11g");
		}
	} else {
		snprintf(wrqu->name, IFNAMSIZ, "unassociated");
	}
	return 0;
}