static struct iw_statistics *rtw_get_wireless_stats(struct net_device *dev)
{
	struct adapter *padapter = rtw_netdev_priv(dev);
	struct iw_statistics *piwstats = &padapter->iwstats;
	int tmp_level = 0;
	int tmp_qual = 0;
	int tmp_noise = 0;

	if (!check_fwstate(&padapter->mlmepriv, _FW_LINKED)) {
		piwstats->qual.qual = 0;
		piwstats->qual.level = 0;
		piwstats->qual.noise = 0;
	} else {
		tmp_level = padapter->recvpriv.signal_strength;
		tmp_qual = padapter->recvpriv.signal_qual;
		tmp_noise = padapter->recvpriv.noise;

		piwstats->qual.level = tmp_level;
		piwstats->qual.qual = tmp_qual;
		piwstats->qual.noise = tmp_noise;
	}
	piwstats->qual.updated = IW_QUAL_ALL_UPDATED;/* IW_QUAL_DBM; */
	return &padapter->iwstats;
}