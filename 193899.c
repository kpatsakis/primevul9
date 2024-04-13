int cfg80211_mgd_wext_siwap(struct net_device *dev,
			    struct iw_request_info *info,
			    struct sockaddr *ap_addr, char *extra)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wdev->wiphy);
	u8 *bssid = ap_addr->sa_data;
	int err;

	/* call only for station! */
	if (WARN_ON(wdev->iftype != NL80211_IFTYPE_STATION))
		return -EINVAL;

	if (ap_addr->sa_family != ARPHRD_ETHER)
		return -EINVAL;

	/* automatic mode */
	if (is_zero_ether_addr(bssid) || is_broadcast_ether_addr(bssid))
		bssid = NULL;

	wdev_lock(wdev);

	if (wdev->conn) {
		err = 0;
		/* both automatic */
		if (!bssid && !wdev->wext.connect.bssid)
			goto out;

		/* fixed already - and no change */
		if (wdev->wext.connect.bssid && bssid &&
		    ether_addr_equal(bssid, wdev->wext.connect.bssid))
			goto out;

		err = cfg80211_disconnect(rdev, dev,
					  WLAN_REASON_DEAUTH_LEAVING, false);
		if (err)
			goto out;
	}

	if (bssid) {
		memcpy(wdev->wext.bssid, bssid, ETH_ALEN);
		wdev->wext.connect.bssid = wdev->wext.bssid;
	} else
		wdev->wext.connect.bssid = NULL;

	err = cfg80211_mgd_wext_connect(rdev, wdev);
 out:
	wdev_unlock(wdev);
	return err;
}