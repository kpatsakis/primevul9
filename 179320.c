static int rtw_ioctl_acl_remove_sta(struct net_device *dev, struct ieee_param *param, int len)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	if (!check_fwstate(pmlmepriv, WIFI_AP_STATE))
		return -EINVAL;

	if (is_broadcast_ether_addr(param->sta_addr))
		return -EINVAL;

	return rtw_acl_remove_sta(padapter, param->sta_addr);
}