int cfg80211_wext_siwgenie(struct net_device *dev,
			   struct iw_request_info *info,
			   struct iw_point *data, char *extra)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wdev->wiphy);
	u8 *ie = extra;
	int ie_len = data->length, err;

	if (wdev->iftype != NL80211_IFTYPE_STATION)
		return -EOPNOTSUPP;

	if (!ie_len)
		ie = NULL;

	wdev_lock(wdev);

	/* no change */
	err = 0;
	if (wdev->wext.ie_len == ie_len &&
	    memcmp(wdev->wext.ie, ie, ie_len) == 0)
		goto out;

	if (ie_len) {
		ie = kmemdup(extra, ie_len, GFP_KERNEL);
		if (!ie) {
			err = -ENOMEM;
			goto out;
		}
	} else
		ie = NULL;

	kfree(wdev->wext.ie);
	wdev->wext.ie = ie;
	wdev->wext.ie_len = ie_len;

	if (wdev->conn) {
		err = cfg80211_disconnect(rdev, dev,
					  WLAN_REASON_DEAUTH_LEAVING, false);
		if (err)
			goto out;
	}

	/* userspace better not think we'll reconnect */
	err = 0;
 out:
	wdev_unlock(wdev);
	return err;
}