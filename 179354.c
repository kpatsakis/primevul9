static int wpa_supplicant_ioctl(struct net_device *dev, struct iw_point *p)
{
	struct ieee_param *param;
	uint ret = 0;

	if (!p->pointer || p->length != sizeof(struct ieee_param))
		return -EINVAL;

	param = memdup_user(p->pointer, p->length);
	if (IS_ERR(param))
		return PTR_ERR(param);

	switch (param->cmd) {
	case IEEE_CMD_SET_WPA_PARAM:
		ret = wpa_set_param(dev, param->u.wpa_param.name, param->u.wpa_param.value);
		break;

	case IEEE_CMD_SET_WPA_IE:
		ret =  rtw_set_wpa_ie(rtw_netdev_priv(dev),
				      (char *)param->u.wpa_ie.data, (u16)param->u.wpa_ie.len);
		break;

	case IEEE_CMD_SET_ENCRYPTION:
		ret = wpa_set_encryption(dev, param, p->length);
		break;

	case IEEE_CMD_MLME:
		ret = wpa_mlme(dev, param->u.mlme.command, param->u.mlme.reason_code);
		break;

	default:
		DBG_88E("Unknown WPA supplicant request: %d\n", param->cmd);
		ret = -EOPNOTSUPP;
		break;
	}

	if (ret == 0 && copy_to_user(p->pointer, param, p->length))
		ret = -EFAULT;

	kfree(param);
	return ret;
}