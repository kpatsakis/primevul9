static int rtw_set_beacon(struct net_device *dev, struct ieee_param *param, int len)
{
	int ret = 0;
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	unsigned char *pbuf = param->u.bcn_ie.buf;

	DBG_88E("%s, len =%d\n", __func__, len);

	if (!check_fwstate(pmlmepriv, WIFI_AP_STATE))
		return -EINVAL;

	memcpy(&pstapriv->max_num_sta, param->u.bcn_ie.reserved, 2);

	if ((pstapriv->max_num_sta > NUM_STA) || (pstapriv->max_num_sta <= 0))
		pstapriv->max_num_sta = NUM_STA;

	if (rtw_check_beacon_data(padapter, pbuf, len - 12 - 2) == _SUCCESS) /* 12 = param header, 2:no packed */
		ret = 0;
	else
		ret = -EINVAL;

	return ret;
}