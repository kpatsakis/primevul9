static int rtw_ioctl_set_macaddr_acl(struct net_device *dev, struct ieee_param *param, int len)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	if (!check_fwstate(pmlmepriv, WIFI_AP_STATE))
		return -EINVAL;

	rtw_set_macaddr_acl(padapter, param->u.mlme.command);

	return 0;
}