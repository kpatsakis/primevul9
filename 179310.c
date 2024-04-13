static int rtw_hostapd_sta_flush(struct net_device *dev)
{
	struct adapter *padapter = rtw_netdev_priv(dev);

	DBG_88E("%s\n", __func__);

	flush_all_cam_entry(padapter);	/* clear CAM */

	return rtw_sta_flush(padapter);
}