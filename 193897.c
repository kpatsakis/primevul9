int cfg80211_mgd_wext_connect(struct cfg80211_registered_device *rdev,
			      struct wireless_dev *wdev)
{
	struct cfg80211_cached_keys *ck = NULL;
	const u8 *prev_bssid = NULL;
	int err, i;

	ASSERT_RTNL();
	ASSERT_WDEV_LOCK(wdev);

	if (!netif_running(wdev->netdev))
		return 0;

	wdev->wext.connect.ie = wdev->wext.ie;
	wdev->wext.connect.ie_len = wdev->wext.ie_len;

	/* Use default background scan period */
	wdev->wext.connect.bg_scan_period = -1;

	if (wdev->wext.keys) {
		wdev->wext.keys->def = wdev->wext.default_key;
		if (wdev->wext.default_key != -1)
			wdev->wext.connect.privacy = true;
	}

	if (!wdev->wext.connect.ssid_len)
		return 0;

	if (wdev->wext.keys && wdev->wext.keys->def != -1) {
		ck = kmemdup(wdev->wext.keys, sizeof(*ck), GFP_KERNEL);
		if (!ck)
			return -ENOMEM;
		for (i = 0; i < CFG80211_MAX_WEP_KEYS; i++)
			ck->params[i].key = ck->data[i];
	}

	if (wdev->wext.prev_bssid_valid)
		prev_bssid = wdev->wext.prev_bssid;

	err = cfg80211_connect(rdev, wdev->netdev,
			       &wdev->wext.connect, ck, prev_bssid);
	if (err)
		kzfree(ck);

	return err;
}