int cfg80211_mgd_wext_siwessid(struct net_device *dev,
			       struct iw_request_info *info,
			       struct iw_point *data, char *ssid)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wdev->wiphy);
	size_t len = data->length;
	int err;

	/* call only for station! */
	if (WARN_ON(wdev->iftype != NL80211_IFTYPE_STATION))
		return -EINVAL;

	if (!data->flags)
		len = 0;

	/* iwconfig uses nul termination in SSID.. */
	if (len > 0 && ssid[len - 1] == '\0')
		len--;

	wdev_lock(wdev);

	err = 0;

	if (wdev->conn) {
		bool event = true;

		if (wdev->wext.connect.ssid && len &&
		    len == wdev->wext.connect.ssid_len &&
		    memcmp(wdev->wext.connect.ssid, ssid, len) == 0)
			goto out;

		/* if SSID set now, we'll try to connect, avoid event */
		if (len)
			event = false;
		err = cfg80211_disconnect(rdev, dev,
					  WLAN_REASON_DEAUTH_LEAVING, event);
		if (err)
			goto out;
	}

	wdev->wext.prev_bssid_valid = false;
	wdev->wext.connect.ssid = wdev->wext.ssid;
	memcpy(wdev->wext.ssid, ssid, len);
	wdev->wext.connect.ssid_len = len;

	wdev->wext.connect.crypto.control_port = false;
	wdev->wext.connect.crypto.control_port_ethertype =
					cpu_to_be16(ETH_P_PAE);

	err = cfg80211_mgd_wext_connect(rdev, wdev);
 out:
	wdev_unlock(wdev);
	return err;
}