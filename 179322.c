static int rtw_wx_set_mode(struct net_device *dev, struct iw_request_info *a,
			   union iwreq_data *wrqu, char *b)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	enum ndis_802_11_network_infra networkType;
	int ret = 0;

	if (!rtw_pwr_wakeup(padapter)) {
		ret = -EPERM;
		goto exit;
	}

	if (!padapter->hw_init_completed) {
		ret = -EPERM;
		goto exit;
	}

	switch (wrqu->mode) {
	case IW_MODE_AUTO:
		networkType = Ndis802_11AutoUnknown;
		DBG_88E("set_mode = IW_MODE_AUTO\n");
		break;
	case IW_MODE_ADHOC:
		networkType = Ndis802_11IBSS;
		DBG_88E("set_mode = IW_MODE_ADHOC\n");
		break;
	case IW_MODE_MASTER:
		networkType = Ndis802_11APMode;
		DBG_88E("set_mode = IW_MODE_MASTER\n");
		break;
	case IW_MODE_INFRA:
		networkType = Ndis802_11Infrastructure;
		DBG_88E("set_mode = IW_MODE_INFRA\n");
		break;
	default:
		ret = -EINVAL;
		RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_err_, ("\n Mode: %s is not supported\n", iw_operation_mode[wrqu->mode]));
		goto exit;
	}
	if (!rtw_set_802_11_infrastructure_mode(padapter, networkType)) {
		ret = -EPERM;
		goto exit;
	}
	rtw_setopmode_cmd(padapter, networkType);
exit:
	return ret;
}