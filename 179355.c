static int rtw_hostapd_ioctl(struct net_device *dev, struct iw_point *p)
{
	struct ieee_param *param;
	int ret = 0;
	struct adapter *padapter = rtw_netdev_priv(dev);

	/*
	 * this function is expect to call in master mode, which allows no power saving
	 * so, we just check hw_init_completed
	 */

	if (!padapter->hw_init_completed)
		return -EPERM;

	if (!p->pointer || p->length != sizeof(struct ieee_param))
		return -EINVAL;

	param = memdup_user(p->pointer, p->length);
	if (IS_ERR(param))
		return PTR_ERR(param);

	switch (param->cmd) {
	case RTL871X_HOSTAPD_FLUSH:
		ret = rtw_hostapd_sta_flush(dev);
		break;
	case RTL871X_HOSTAPD_ADD_STA:
		ret = rtw_add_sta(dev, param);
		break;
	case RTL871X_HOSTAPD_REMOVE_STA:
		ret = rtw_del_sta(dev, param);
		break;
	case RTL871X_HOSTAPD_SET_BEACON:
		ret = rtw_set_beacon(dev, param, p->length);
		break;
	case RTL871X_SET_ENCRYPTION:
		ret = rtw_set_encryption(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_GET_WPAIE_STA:
		ret = rtw_get_sta_wpaie(dev, param);
		break;
	case RTL871X_HOSTAPD_SET_WPS_BEACON:
		ret = rtw_set_wps_beacon(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_SET_WPS_PROBE_RESP:
		ret = rtw_set_wps_probe_resp(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_SET_WPS_ASSOC_RESP:
		ret = rtw_set_wps_assoc_resp(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_SET_HIDDEN_SSID:
		ret = rtw_set_hidden_ssid(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_GET_INFO_STA:
		ret = rtw_ioctl_get_sta_data(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_SET_MACADDR_ACL:
		ret = rtw_ioctl_set_macaddr_acl(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_ACL_ADD_STA:
		ret = rtw_ioctl_acl_add_sta(dev, param, p->length);
		break;
	case RTL871X_HOSTAPD_ACL_REMOVE_STA:
		ret = rtw_ioctl_acl_remove_sta(dev, param, p->length);
		break;
	default:
		DBG_88E("Unknown hostapd request: %d\n", param->cmd);
		ret = -EOPNOTSUPP;
		break;
	}

	if (ret == 0 && copy_to_user(p->pointer, param, p->length))
		ret = -EFAULT;
	kfree(param);
	return ret;
}